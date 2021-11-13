import argparse
import json
import logging
import os
import re
import shutil
import subprocess
import sys
import time

import esptool
import toml

import spiffsgen

logging.basicConfig(format='- [%(levelname)s]: %(message)s', level=logging.INFO)


def flashFs(fspath, port, baud, chip, offset, size):
    if chip == "esp32c3" or chip == "esp32s3":
        if getattr(sys, 'frozen', False):
            bundle_dir = sys._MEIPASS
        else:
            bundle_dir = os.path.dirname(os.path.abspath(__file__))
        # luac
        if not os.path.exists('tmp'):
            os.mkdir('tmp')
        else:
            shutil.rmtree('tmp')
            os.mkdir('tmp')
        for root, dirs, name in os.walk(fspath):
            for i in range(len(name)):
                if name[i].endswith(".lua"):
                    cmd = bundle_dir + "\\bin\\luac_536_32bits.exe -o" + " tmp/" + \
                          os.path.basename(name[i]) + "c " + fspath + os.path.basename(name[i])
                    # print(cmd)
                    os.system(cmd)
                # 其他文件直接拷贝
                else:
                    shutil.copy(fspath + name[i], "tmp/" + os.path.basename(name[i]))

        # 制作fs分区
        with open('spiffs.bin', 'wb') as image_file:
            image_size = int(size, 0)
            spiffs_build_default = spiffsgen.SpiffsBuildConfig(256, spiffsgen.SPIFFS_PAGE_IX_LEN,
                                                               4096, spiffsgen.SPIFFS_BLOCK_IX_LEN, 4,
                                                               32, spiffsgen.SPIFFS_OBJ_ID_LEN,
                                                               spiffsgen.SPIFFS_SPAN_IX_LEN,
                                                               True, True, 'little',
                                                               True, True)
            spiffs = spiffsgen.SpiffsFS(image_size, spiffs_build_default)
            for root, dirs, files in os.walk("tmp/", followlinks=False):
                for f in files:
                    full_path = os.path.join(root, f)
                    spiffs.create_file('/' + os.path.relpath(full_path, "tmp/").replace('\\', '/'),
                                       full_path)
            image = spiffs.to_binary()
            image_file.write(image)
    else:
        logging.error("not support chip")
        sys.exit(-1)

    command = ['--port', port,
               '--baud', baud,
               '--chip', chip,
               'write_flash',
               offset,
               "spiffs.bin"]
    esptool.main(command)
    # 最后删除临时目录
    shutil.rmtree("tmp")


def pkgRom(chip):
    if chip == "esp32c3" or chip == "esp32s3":
        # 查找固件位置
        flasher_args = open(config['pkg']['Repo'] + 'build/' + "flasher_args.json", 'r', encoding='utf-8')
        j = json.load(flasher_args)
        if j['extra_esptool_args']['chip'] != chip:
            logging.error("The selected chip is inconsistent with the build")
            sys.exit(-1)
        ss = sorted(
            ((o, f) for (o, f) in j['flash_files'].items()),
            key=lambda x: int(x[0], 0),
        )

        # 获取版本信息
        logging.info("start get version")
        with open(config['pkg']['Repo'] + 'components/luat/include/luat_base.h', 'r', encoding='utf-8') as f:
            for line in f.readlines():
                line = line.strip('\n')
                if re.match('#define LUAT_VERSION', line):
                    vc = re.sub('^#define LUAT_VERSION', '', line).strip()
                    versionCore = re.sub(r'"(?!")', '', vc)
                    break
        logging.info("versionCore:{}".format(versionCore))

        with open(config['pkg']['Repo'] + 'components/luat/include/luat_conf_bsp.h', 'r', encoding='utf-8') as f:
            for line in f.readlines():
                line = line.strip('\n')
                if re.match('#define LUAT_BSP_VERSION', line):
                    vb = re.sub('^#define LUAT_BSP_VERSION', '', line).strip()
                    versionBsp = re.sub(r'"(?!")', '', vb)
                    break
        logging.info("versionBsp:{}".format(versionBsp))

        # 判断打包类型
        if config['pkg']['Release'] == 0:
            logging.warning("user build")
            git_sha1 = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).strip()
            firmware_name = "LuatOS-SoC_" + chip + '_' + \
                            git_sha1.decode() + "_" + \
                            time.strftime("%Y%m%d%H%M%S", time.localtime()) + ".bin"
        elif config['pkg']['Release'] == 1:
            logging.warning("release build")
            firmware_name = "LuatOS-SoC_" + chip + '_' + versionBsp + ".bin"
        else:
            logging.error("release option error")
            sys.exit(-1)

        # 进入合并流程
        base_offset = 0x0
        with open(firmware_name, "wb") as fout:
            for offset, name in ss:
                fout.write(b"\xff" * (int(offset, 16) - base_offset))
                base_offset = int(offset, 16)
                with open(config['pkg']['Repo'] + 'build/' + name, "rb") as fin:
                    data = fin.read()
                    fout.write(data)
                    base_offset += len(data)
    else:
        logging.error("not support chip")
        sys.exit(-1)


def flashRom(rom, port, baud, chip):
    if chip == "esp32c3" or chip == "esp32s3":
        logging.info("erase flash")
        command_erase = ['--port', port,
                         '--baud', baud,
                         '--chip', chip,
                         'erase_flash']
        esptool.main(command_erase)

        logging.info("start flash firmware")
        command = ['--port', port,
                   '--baud', str(baud),
                   '--chip', chip,
                   'write_flash',
                   '0x0',
                   rom]
        print(command)
        esptool.main(command)
    else:
        logging.error("not support chip")
        sys.exit(-1)


def get_version():
    return '3.0.0'


if __name__ == '__main__':
    if not os.path.exists("config.toml"):
        logging.error("config.toml not found ,please check")
        sys.exit(-1)
    config = toml.load("config.toml")
    parser = argparse.ArgumentParser(description="ESP32 Flash Tool")
    parser.add_argument('-v', '--version', action='version', version=get_version(), help='Show version')
    parser.add_argument('-t', '--target', help='Chip型号:es32c3,esp32s3')
    parser.add_argument('-f', '--fs', action="store_true", help='下载脚本')
    parser.add_argument('-r', '--rom', action="store_true", help='下载底层固件')
    parser.add_argument('-p', '--pkg', action="store_true", help='打包固件')
    args = parser.parse_args()

    ChipName = args.target
    if ChipName is None:
        logging.error("未选择chip")
        sys.exit(-1)
    if args.pkg:
        pkgRom(ChipName)
    if args.rom:
        flashRom(config[ChipName]['Firmware'],
                 config[ChipName]['COM'],
                 config[ChipName]['Baud'],
                 ChipName)
    if args.fs:
        flashFs(config[ChipName]['FsPath'],
                config[ChipName]['COM'],
                config[ChipName]['Baud'],
                ChipName,
                config[ChipName]['FsOffset'],
                config[ChipName]['FsSize'])
