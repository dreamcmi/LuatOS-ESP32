import json
import os
import shutil
import sys
import zipfile
import configparser

try:
    import esptool

    print("import esptool")
except:
    print("Please install esptool")
    print("请执行pip install esptool")


# 生成标准固件（不包括fs分区）
def genera_firmware_pkg():
    isExists = os.path.exists('tmp')
    if not isExists:
        # 判断下tmp文件夹，有就创建，没有就跳过
        os.mkdir('tmp')
        print("mkdir /tmp")
    else:
        print("found /tmp")
    # 检索build里面的分区表
    with open(user_build_path + '/flasher_args.json', 'r', encoding='utf-8') as f:
        firmware_json = json.load(f)
        # print(firmware_json)
        bootloader_offset = firmware_json['bootloader']['offset']
        bootloader_file = firmware_json['bootloader']['file']
        partition_table_offset = firmware_json['partition_table']['offset']
        partition_table_file = firmware_json['partition_table']['file']
        otadata_offset = firmware_json['otadata']['offset']
        otadata_file = firmware_json['otadata']['file']
        app_offset = firmware_json['app']['offset']
        app_file = firmware_json['app']['file']
        print("read partition ok")

    # 将读到的分区表写入配置
    info = {
        'type': 'esp32',
        'bootloader_offset': bootloader_offset,
        'bootloader_file': "bootloader.bin",
        'partition_table_offset': partition_table_offset,
        'partition_table_file': "partition-table.bin",
        'otadata_offset': otadata_offset,
        'otadata_file': otadata_file,
        'app_offset': app_offset,
        'app_file': app_file
    }
    info_j = json.dumps(info)
    with open('./tmp/rominfo.json', 'w') as f:
        data = info_j
        # 这里会覆盖哟
        f.write(data)
        print("write tmp/rominfo.json done")

    # 复制bin到tmp目录
    shutil.copy(user_build_path + '/' + bootloader_file, "tmp/")
    shutil.copy(user_build_path + '/' + partition_table_file, "tmp/")
    shutil.copy(user_build_path + '/' + otadata_file, "tmp/")
    shutil.copy(user_build_path + '/' + app_file, "tmp/")
    print("copy down")

    # 打包pkg
    z = zipfile.ZipFile("Luatos_esp32.zip", "w")
    if os.path.isdir("tmp"):
        for d in os.listdir("tmp"):
            z.write("tmp/" + d, arcname=d)
            print("ziping:" + d)
    z.close()
    print("pkg done")
    # tmp没用了，删了
    shutil.rmtree("tmp")
    print("rm ./tmp")


# 解压固件
def unzip_firmware():
    zfile = zipfile.ZipFile(firmware_path, "r")
    for f in zfile.namelist():
        print(f)
    zfile.extractall('./tmp')
    print("unzip done")


# 刷写固件
def flash_target():
    # 先把固件说明读出来
    with open('./tmp/rominfo.json', 'r', encoding='utf8')as fp:
        rom_info_data = json.load(fp)
        # 判断下固件是不是给esp32的
        if rom_info_data["type"] == "esp32":
            print("this is a esp32 fimware")

            bootloader_offset = rom_info_data['bootloader_offset']
            bootloader_file = './tmp/' + rom_info_data['bootloader_file']

            partition_table_offset = rom_info_data['partition_table_offset']
            partition_table_file = './tmp/' + rom_info_data['partition_table_file']

            otadata_offset = rom_info_data['otadata_offset']
            otadata_file = './tmp/' + rom_info_data['otadata_file']

            app_offset = rom_info_data['app_offset']
            app_file = './tmp/' + rom_info_data['app_file']

            command = ['--port', user_com, '--baud', user_baud, 'write_flash',
                       bootloader_offset, bootloader_file,
                       partition_table_offset, partition_table_file,
                       otadata_offset, otadata_file,
                       app_offset, app_file]

            esptool.main(command)

        else:
            print("This is not a esp32 firmware")
    # tmp没用了，删了
    shutil.rmtree("tmp")
    print("rm ./tmp")


# 生成luat脚本刷写文件
def make_luat_fs():
    os.system("python spiffsgen.py " + fs_size + " " + fs_path + " " + fs_bin)
    print("spiffs gen done")


# 下载luat脚本
def dl_fs():
    command = ['--port', user_com, '--baud', user_baud, 'write_flash', user_baud, fs_bin]
    esptool.main(command)


def main():
    argc = 1
    while len(sys.argv) > argc:
        if sys.argv[argc] == "lfs":
            print("Action mklfs -----------------------------------------")
            make_luat_fs()
        elif sys.argv[argc] == "dlfs":
            print("Burn FS 下载文件系统 -----------------------------------")
            dl_fs()
        elif sys.argv[argc] == "dlrom":  # 下载底层
            print("Burn ROM 下载固件 -------------------------------------")
            unzip_firmware()
            flash_target()
        elif sys.argv[argc] == "pkg":
            print("Action pkg 打包固件------------------------------------")
            genera_firmware_pkg()
        else:
            print("输入指令有错误诶，检查一下吧")
        argc += 1
        print("============================================================================")
    print("every done, bye")


if __name__ == '__main__':
    config = configparser.ConfigParser()
    config['esp32'] = {
        # ============================================================
        #
        #          配置信息是读取local.ini为主的,以下的只是默认配置.
        #          请修改local.ini, 一般不需要修改本脚本里的配置信息.
        #
        # ============================================================
        # ============================================================
        "USER_BUILD_PATH": "D:\\gitee\\esp32\\LuatOS-ESP32-gitee\\build\\",
        "FIRMWARE_PATH": "Luatos_esp32.zip",
        "DEMO_PATH": "\\demo\\",
        "FS_OFFSET": "0X310000",
        "FS_SIZE": "0XF000",
        "FS_BIN": "demo.bin",
        "COM_BAUD": "921600",
        "COM_PORT": "COM52"  # 请修改local.ini文件
    }
    if os.path.exists("local.ini"):
        config.read("local.ini")
        user_build_path = os.path.abspath(config['esp32']['USER_BUILD_PATH']) + os.sep
        firmware_path = os.path.abspath(config['esp32']['FIRMWARE_PATH'])
        fs_path = os.path.abspath(config['esp32']['DEMO_PATH']) + os.sep
        fs_offset = config['esp32']['FS_OFFSET']
        fs_size = config['esp32']['FS_SIZE']
        fs_bin = os.path.abspath(config['esp32']['FS_BIN'])
        user_baud = config['esp32']['COM_BAUD']
        user_com = config['esp32']['COM_PORT']
        print("读取local.ini成功")
        # print(user_build_path+" "+firmware_path+" "+fs_path+" "+fs_offset+" "+fs_size+" "+fs_bin+" "+user_baud+" "+user_com)
    else:
        print("没找到local.ini,请创建一个")

    if len(sys.argv) == 1:
        # 等于1就是没输入操作命令呗
        print('''
        欢迎使用Luatos For ESP32 Flash Tool
        Author:梦程MI(Darren)
        Version:V1.1
        下面是使用说明
        -------------------------------------
        pkg   - 生成标准固件（不包括fs分区）
        pack  - 生成量产固件（暂未支持）
        dlrom - 刷写固件
        dlfs  - 刷写脚本
        lfs   - 生成脚本刷写文件
        --------------------------------------
        用例1, 生成文件系统
        python esp32.py lfs
        
        用例2, 生成文件系统并下载到开发板
        python esp32.py lfs dlfs

        用例3, 仅下载底层固件
        python esp32.py dlrom

        用例4, 生成标准固件
        python esp32.py pkg
        --------------------------------------
        ''')
    else:
        main()