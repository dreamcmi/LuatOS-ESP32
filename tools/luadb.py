import os
import shutil
import traceback
import re
import sys
import time
import struct

import binascii
import subprocess
import hashlib
from pathlib import Path

def sum_check(bdata):
    result = 0
    for i in bdata:
        result += i
    return result

def encode_ch(string, d='gbk'):
    if type(string) is not str:
        return string
    try:
        return string.encode(d)
    except Exception as Error1:
        try:
            if d == 'gbk':
                return string.encode('utf-8')
            else:
                return string.encode('gbk')
        except Exception as Error2:
            return string.encode(d, errors='ignore')

def resource_path(relative_path):
    if getattr(sys, 'frozen', False):
        base_path = sys._MEIPASS
    else:
        base_path = os.getcwd()
    return str(Path(base_path, relative_path))

class CheckScript():
    def __init__(self):
        self.all_file_crc_name = ".airm2m_all_crc#.bin"

    def merge_add_head(self):
            self.bin_data = bytearray()
            magic_mag = struct.pack("<BBI", 0x01, 0x04, 0xA55AA55A)
            # if self.support_luadb_v2():
            version_mag = struct.pack("<BBh", 0x02, 0x02, 0x2)
            length_msg = struct.pack("<BBI", 0x03, 0x04, 0x18)
            file_num_msg = struct.pack("<BBH", 0x04, 0x02, len(self.all_files) + 1)
            crc_head = struct.pack("<BB", 0xFE, 0x02)
            data = magic_mag + version_mag + length_msg + file_num_msg + crc_head
            self.bin_data.extend(data)
            num = sum_check(data)
            self.bin_data.extend(struct.pack("<H", num))

    def merge_add_body(self, dir_path, file_name=None):                 # 添加头文件的单个文件
            if not file_name:
                file_name = os.path.basename(dir_path)
                dir_path = os.path.dirname(dir_path)
            magic_mag = struct.pack("<BBI", 0x01, 0x04, 0xA55AA55A)
            n = len(file_name)
            name_msg = struct.pack("<BB%is" % n, 0x02, n, encode_ch(file_name, d='utf-8'))
            try:
                with open(Path(dir_path, file_name), "rb") as f:
                    file_data = f.read()
                #print(dir_path, file_name, file_data)
            except Exception as identifier:
                print("error when reading", dir_path, file_name)
                traceback.print_exc()
                return False
            length_msg = struct.pack("<BBI", 0x03, 0x04, len(file_data))
            crc_head = struct.pack("<BB", 0xFE, 0x02)
            data = magic_mag + name_msg + length_msg + crc_head
            self.bin_data.extend(data)
            num = sum_check(data)
            self.bin_data.extend(struct.pack("<H", num))
            self.bin_data.extend(file_data)
            return True

    def merge_add_all_crc(self):                 # 添加全文件CRC 校验
        magic_mag = struct.pack("<BBI", 0x01, 0x04, 0xA55AA55A)
        n = len(self.all_file_crc_name)
        name_msg = struct.pack("<BB%is" % n, 0x02, n, encode_ch(self.all_file_crc_name, d='utf-8'))

        length_msg = struct.pack("<BBI", 0x03, 0x04, 16)
        crc_head = struct.pack("<BB", 0xFE, 0x02)
        data = magic_mag + name_msg + length_msg + crc_head
        self.bin_data.extend(data)
        num = sum_check(data)
        self.bin_data.extend(struct.pack("<H", num))
        soft_md5 = hashlib.md5(self.bin_data).hexdigest()
        if len(soft_md5) != 32:
            raise Exception("md5 check err %d" % len(soft_md5))
        #print(len(self.bin_data), soft_md5)
        self.bin_data.extend(binascii.a2b_hex(soft_md5))
        return True

    def merge_soc_script(self, file_dir, all_files):
        self.merge_add_head()
        for _file in all_files:
            if not self.merge_add_body(file_dir, _file):
                return False, _("合并") + " " + str(_file) + " " + _("时发生错误")
        self.merge_add_all_crc()
        return True, self.bin_data

# 所搜指定目录下的所有文件和文件夹，但是不包括子文件夹下的文件
def get_only_file_from_path(path, suffix=None):
    file_list = []
    if suffix:
        if type(suffix) is list or type(suffix) is dict:
            _check = suffix
        else:
            _check = [suffix]
    if os.path.exists(path):
        for file_path in os.listdir(path):
            file_name, file_suffix = os.path.splitext(file_path)
            if suffix:
                if file_suffix[1:] in _check or file_suffix in _check:
                    file_list.append(file_path)
            else:
                file_list.append(file_path)
    return file_list

def merge(disk_path):
    all_files = get_only_file_from_path(disk_path)
    if len(all_files) == 0 :
        print("None File Found")
        return False
    script = CheckScript()
    script.all_files = all_files
    result, bin_data = script.merge_soc_script(disk_path, all_files)
    if not result:
        return False
    else:
        with open("disk.fs", "wb") as f:
            f.write(bin_data)
        return True

if __name__ == '__main__':
    if str(sys.argv[1]):
        if not merge(Path(sys.argv[1])) :
            print("merge FAIL!!!")
            sys.exit(1)
    else :
        print("need path")
        sys.exit(1)
