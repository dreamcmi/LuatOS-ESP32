# ESP32_LuatOS_Flashtool

## 简介：

本工具是Luatos For ESP32 项目的配套工具，用于固件的生成，下载，量产

Author:梦程MI(Darren)

Version:V1.1


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

local.ini是配置文件，程序会先读取local.ini里面的配置

程序依赖esptool工具，请先使用pip安装

安装命令：pip install esptool

操作命令

```
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
```

