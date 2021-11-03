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
            part_offset = buildJson['partition-table']['offset']
            part_file = buildJson['partition-table']['file']
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
            git_sha1 = subprocess.check_output(
                ['git', 'rev-parse', '--short', 'HEAD']).strip()
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
    return '2.0.2'


if __name__ == '__main__':
    if not os.path.exists("config.toml"):
        logging.error("config.toml not found ,please check")
        sys.exit(-1)
    config = toml.load("config.toml")
    parser = argparse.ArgumentParser(
        description="LuatOS-SoC For ESP32 Flash Tool")
    parser.add_argument('-v', '--version', action='version',
                        version=get_version(), help='Show version')
    parser.add_argument(
        '-t', '--target', help='Chip型号:esp32,es32c3,esp32s2,esp32s3')
    parser.add_argument('-f', '--fs', action="store_true", help='下载脚本')
    parser.add_argument('-r', '--rom', action="store_true", help='下载底层固件')
    parser.add_argument('-p', '--pkg', action="store_true", help='打包固件')
    args = parser.parse_args()

    ChipName = args.target
    if args.pkg:
        pkgRom()
