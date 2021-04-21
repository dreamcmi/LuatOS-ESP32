import json
import os
import shutil
import sys
import esptool
import zipfile


def unzip_firmware():
    zfile = zipfile.ZipFile(firmware_path, "r")
    for f in zfile.namelist():
        print(f)
    zfile.extractall('./firmware')
    print("unzip done")


def update_firmware():
    with open('./firmware/rominfo.json', 'r', encoding='utf8')as fp:
        rom_info_data = json.load(fp)

        if rom_info_data["type"] == "esp32":
            print("this is a esp32 fimware")

            bootloader_offset = rom_info_data['bootloader_offset']
            bootloader_file = './firmware/' + rom_info_data['bootloader_file']

            partition_table_offset = rom_info_data['partition_table_offset']
            partition_table_file = './firmware/' + rom_info_data['partition_table_file']

            otadata_offset = rom_info_data['otadata_offset']
            otadata_file = './firmware/' + rom_info_data['otadata_file']

            app_offset = rom_info_data['app_offset']
            app_file = './firmware/' + rom_info_data['app_file']

            command = ['--port', user_com, '--baud', user_baud, 'write_flash',
                       bootloader_offset, bootloader_file,
                       partition_table_offset, partition_table_file,
                       otadata_offset, otadata_file,
                       app_offset, app_file]

            esptool.main(command)
        else:
            print("This is not a esp32 firmware")


def del_firmware():
    shutil.rmtree('firmware')
    print("rm ./firmware")


def zip_firmware():
    info = {
        "type": "esp32",
        "bootloader_offset": "0x1000",
        "bootloader_file": "bootloader.bin",
        "partition_table_offset": "0x8000",
        "partition_table_file": "partition-table.bin",
        "otadata_offset": "0xe000",
        "otadata_file": "ota_data_initial.bin",
        "app_offset": "0x10000",
        "app_file": "luatos_esp32.bin",
        "spiffs_offset": "0xf0000",
        "spiffs_file": "demo.bin"
    }
    info_j = json.dumps(info)
    with open('./rominfo.json', 'w') as f:
        data = info_j
        f.write(data)

    zfile = zipfile.ZipFile("luatos_esp32.zip", "w")
    zfile.write(user_build_path + "/luatos_esp32.bin", arcname='luatos_esp32.bin')
    zfile.write(user_build_path + "/ota_data_initial.bin", arcname='ota_data_initial.bin')
    zfile.write(user_build_path + "/bootloader/bootloader.bin", arcname='bootloader.bin')
    zfile.write(user_build_path + "/partition_table/partition-table.bin", arcname='partition-table.bin')
    zfile.write("rominfo.json")
    zfile.close()
    os.remove('rominfo.json')


def lfs_lua():
    os.system('mkspiffs -c ' + user_lua_src + ' -b 4096 -p 256 -s 0xF0000 demo.bin')


if __name__ == '__main__':
    user_mode = sys.argv[1]

    if user_mode == "help":
        print("ESP32 flash tool")
        print("-------------------------------------------------")
        print("lfs [lua_path]")
        print("生成脚本刷入文件")
        print("-------------------------------------------------")
        print("flash [COM] [baud] [firmware_path] ")
        print("下载固件")
        print("-------------------------------------------------")
        print("pkg [build_path] ")
        print("生成固件包")

    elif user_mode == "flash":
        user_com = sys.argv[2]
        user_baud = sys.argv[3]
        firmware_path = sys.argv[4]
        unzip_firmware()
        update_firmware()
        del_firmware()

    elif user_mode == "pkg":
        user_build_path = sys.argv[2]
        # user_lua_path = sys.argv[3]
        zip_firmware()

    elif user_mode == "lfs":
        user_lua_src = sys.argv[2]
        lfs_lua()
