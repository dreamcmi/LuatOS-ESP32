# ESP32_LuatOS_Flashtool

## 简介：

本工具是Luatos For ESP32 项目的配套工具，用于固件的生成，下载，量产

Author:梦程MI(Darren)

Version:V1.1.1（Date：2020.4.23）


## 1、鉴别格式：rominfo.json

```json
{
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
```

## 2、使用说明

#### 配置文件

local.ini是配置文件，程序会先读取local.ini里面的配置

以下是local.ini示例格式

```ini
[esp32]
ESP_IDF_PATH : D:\gitee\esp32\esp-idf
;esp-idf安装目录
USER_PROJECT_PATH : D:\gitee\esp32\LuatOS-ESP32-gitee
;项目根目录（LuatOS for ESP32仓库根目录）
FIRMWARE_PATH : Luatos_esp32_batch_d53c5b3_20210423234021.zip
;固件路径
DEMO_PATH : demo
;用户脚本的路径
FS_OFFSET : 0x310000
;spiffs分区的偏移地址
FS_SIZE : 0XF0000
;spiffs分区大小
FS_BIN : demo.bin
;生成刷入脚本文件的名称
COM_BAUD : 921600
;刷写波特率
COM_PORT : COM52
;刷写端口
```

#### 依赖关系

程序依赖esptool工具，请先使用pip安装

安装命令：pip install esptool

#### 操作命令

```
        -------------------------------------
        pkg   - 生成标准固件（不包括fs分区）
        pack  - 生成量产固件（包括fs分区）
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
        
        用例5, 生成量产固件
        python esp32.py pack
        
        注：参数可以添加多个，依次往后加就行
```

## 3、鸣谢

排名不分前后

- [LuatOS](https://gitee.com/openLuat/LuatOS) ：合宙LuatOS是运行在嵌入式硬件的实时操作系统,用户编写lua代码就可完成各种功能

- [espressif ](https://www.espressif.com/): 乐鑫科技官网，提供高性价比高可玩性的ESP32系列产品

- [Wendal](https://gitee.com/wendal) ：技术大佬，LuatOS领头人

- [Jetbrains](https://www.jetbrains.com/) ：提供Pycharm高效率开发IDE

  

