import argparse
import getpass
import json
import logging
import os
import platform
import re
import shutil
import subprocess
import sys
import time
import zipfile

import esptool
import toml

import spiffsgen

ChipName = ""
versionCore = ""
versionBsp = ""
logging.basicConfig(format='- [%(levelname)s]: %(message)s', level=logging.INFO)


def flashFs():
    if ChipName == "esp32" or ChipName == "esp32c3" or ChipName == "esp32s3":
        # os.system("python spiffsgen.py " + config[ChipName]['FsSize'] + " " + config[ChipName][
        #     'FsPath'] + " " + 'spiffs.bin')
        with open('spiffs.bin', 'wb') as image_file:
            image_size = int(config[ChipName]['FsSize'], 0)
            spiffs_build_default = spiffsgen.SpiffsBuildConfig(256, spiffsgen.SPIFFS_PAGE_IX_LEN,
                                                               4096, spiffsgen.SPIFFS_BLOCK_IX_LEN, 4,
                                                               32, spiffsgen.SPIFFS_OBJ_ID_LEN,
                                                               spiffsgen.SPIFFS_SPAN_IX_LEN,
                                                               True, True, 'little',
                                                               True, True)
            spiffs = spiffsgen.SpiffsFS(image_size, spiffs_build_default)
            for root, dirs, files in os.walk(config[ChipName]['FsPath'], followlinks=False):
                for f in files:
                    full_path = os.path.join(root, f)
                    spiffs.create_file('/' + os.path.relpath(full_path, config[ChipName]['FsPath']).replace('\\', '/'),
                                       full_path)
            image = spiffs.to_binary()
            image_file.write(image)
    else:
        logging.error("not support chip")
        sys.exit(-1)

    command_erase = ['--port', config[ChipName]['COM'],
                     '--chip', ChipName, 'erase_region',
                     config[ChipName]['FsOffset'],
                     config[ChipName]['FsSize']]
    esptool.main(command_erase)

    command = ['--port', config[ChipName]['COM'],
               '--baud', str(config[ChipName]['Baud']),
               '--chip', ChipName,
               'write_flash',
               config[ChipName]['FsOffset'],
               "spiffs.bin"]
    esptool.main(command)


def flashRom():
    if ChipName == "esp32" or ChipName == "esp32c3" or ChipName == "esp32s3":
        if not os.path.exists('tmp'):
            os.mkdir('tmp')
        else:
            shutil.rmtree('tmp')
            os.mkdir('tmp')
        zfile = zipfile.ZipFile(config[ChipName]['Firmware'], "r")
        zfile.extractall('./tmp')
        logging.info("unzip done")
        with open('./tmp/info.json', 'r', encoding='utf-8') as f:
            fir_info = json.load(f)
            if fir_info['chip']['type'] != ChipName:
                logging.error("firmware not correct")
                shutil.rmtree("tmp")
                sys.exit(-1)
            else:
                bl_offset = fir_info['rom']['fs']['bootloader']['offset']
                bl_file = fir_info['rom']['fs']['bootloader']['name']

                par_offset = fir_info['rom']['fs']['partition_table']['offset']
                par_file = fir_info['rom']['fs']['partition_table']['name']

                ota_offset = fir_info['rom']['fs']['otadata']['offset']
                ota_file = fir_info['rom']['fs']['otadata']['name']

                app_offset = fir_info['rom']['fs']['app']['offset']
                app_file = fir_info['rom']['fs']['app']['name']

                command = ['--port', config[ChipName]['COM'],
                           '--baud', str(config[ChipName]['Baud']),
                           '--chip', ChipName,
                           'write_flash',
                           bl_offset, './tmp/' + bl_file,
                           par_offset, './tmp/' + par_file,
                           ota_offset, './tmp/' + ota_file,
                           app_offset, './tmp/' + app_file]

        logging.info("erase flash")
        command_erase = ['--port', config[ChipName]['COM'],
                         '--baud', str(config[ChipName]['Baud']),
                         '--chip', ChipName,
                         'erase_flash']
        esptool.main(command_erase)
        logging.info("start flash firmware")
        esptool.main(command)
        shutil.rmtree("tmp")
    else:
        logging.error("not support chip")
        sys.exit(-1)


def pkgRom():
    if not os.path.exists(config['pkg']['Repo'] + 'build/'):
        logging.error("not build")
        sys.exit(-1)
    else:
        buildPath = config['pkg']['Repo'] + 'build/'
        # --------------------------------
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
        # ---------------------------------
        if not os.path.exists('tmp'):
            os.mkdir('tmp')
        else:
            shutil.rmtree('tmp')
            os.mkdir('tmp')
        with open(buildPath + 'flasher_args.json', 'r', encoding='utf-8') as f:
            buildJson = json.load(f)
            part_offset = buildJson['partition_table']['offset']
            part_file = buildJson['partition_table']['file']
            ota_offset = buildJson['otadata']['offset']
            ota_file = buildJson['otadata']['file']
            bl_offset = buildJson['bootloader']['offset']
            bl_file = buildJson['bootloader']['file']
            app_offset = buildJson['app']['offset']
            app_file = buildJson['app']['file']
            chip_name = buildJson['extra_esptool_args']['chip']

        shutil.copy(buildPath + part_file, 'tmp/')
        shutil.copy(buildPath + ota_file, 'tmp/')
        shutil.copy(buildPath + bl_file, 'tmp/')
        shutil.copy(buildPath + app_file, 'tmp/')

        info = {
            "version": 1,
            "chip": {
                "type": chip_name
            },
            "rom": {
                "app_file": app_file,
                "fs": {
                    "script": {
                        "name": "spiffs.bin",
                        "offset": config[chip_name]['FsOffset'],
                        "size": 0
                    },
                    "bootloader": {
                        "name": "bootloader.bin",
                        "offset": bl_offset,
                        "size": os.path.getsize(buildPath + bl_file)
                    },
                    "partition_table": {
                        "name": "partition-table.bin",
                        "offset": part_offset,
                        "size": os.path.getsize(buildPath + part_file)
                    },
                    "otadata": {
                        "name": "ota_data_initial.bin",
                        "offset": ota_offset,
                        "size": os.path.getsize(buildPath + ota_file)
                    },
                    "app": {
                        "name": app_file,
                        "offset": app_offset,
                        "size": os.path.getsize(buildPath + app_file)
                    }
                },
                "version-core": versionCore,
                "version-bsp": versionBsp,
                "mark": "default",
                "build": {
                    "build_at": time.ctime(os.path.getmtime(buildPath + app_file)),
                    "build_by": getpass.getuser(),
                    "build_host": platform.node(),
                    "build_system": platform.platform()
                }
            },
            "script": {
                "file": "spiffs.bin",
                "lua": "5.3",
                "bitw": 32,
                "use-luac": 'true',
                "use-debug": 'true'
            },
            "user": {
                "project": "",
                "version": ""
            },
            "download": {
                "bootloader_offset": bl_offset,
                "partition_table_offset": part_offset,
                "otadata_offset": ota_offset,
                "app_offset": app_offset,
                "spiffs_offset": config[chip_name]['FsOffset']
            }
        }
        info_j = json.dumps(info)
        with open('./tmp/info.json', 'w') as f:
            f.write(info_j)
        if config['pkg']['Release'] == 0:
            logging.warning("user build")
            git_sha1 = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).strip()
            zip_name = "LuatOS-SoC_" + chip_name + '_' + \
                       git_sha1.decode() + "_" + \
                       time.strftime("%Y%m%d%H%M%S", time.localtime()) + ".soc"
        elif config['pkg']['Release'] == 1:
            logging.warning("release build")
            zip_name = "LuatOS-SoC_" + chip_name + '_' + versionBsp + ".soc"
        else:
            logging.error("release option error")
            sys.exit(-1)

        logging.info(zip_name)
        z = zipfile.ZipFile(zip_name, "w")
        if os.path.isdir("tmp"):
            for d in os.listdir("tmp"):
                z.write("tmp/" + d, arcname=d)
        z.close()
        shutil.rmtree("tmp")
def get_version():
    return '2.0.1'

if __name__ == '__main__':
    if not os.path.exists("config.toml"):
        logging.error("config.toml not found ,please check")
        sys.exit(-1)
    config = toml.load("config.toml")
    parser = argparse.ArgumentParser(description="LuatOS-SoC For ESP32 Flash Tool")
    parser.add_argument('-v', '--version', action='version', version=get_version(), help='Show version')
    parser.add_argument('-t', '--target', help='Chip型号:esp32,es32c3,esp32s2,esp32s3')
    parser.add_argument('-f', '--fs', action="store_true", help='下载脚本')
    parser.add_argument('-r', '--rom', action="store_true", help='下载底层固件')
    parser.add_argument('-p', '--pkg', action="store_true", help='打包固件')
    args = parser.parse_args()

    ChipName = args.target
    if args.pkg:
        pkgRom()
    if args.rom:
        flashRom()
    if args.fs:
        flashFs()
