# ESP32_LuatOS_Flashtool

## 简介：

本工具是Luatos For ESP32 项目的配套工具，用于固件的生成，下载，量产

Author : 梦程MI(Darren)

Version : V2.0.0（Date：2021.10.23）




## 1、使用说明

#### 配置文件

配置文件:`config.toml`   [点击了解TOML](https://github.com/toml-lang/toml)

```toml
[pkg]
Repo = 'D:\gitee\esp32\LuatOS-ESP32\'

[esp32c3]
Type = 'uart' #'uart' or 'usb'
COM = 'COM176'
Baud = 921600
Firmware = 'LuatOS-SoC_esp32c3_V0001.soc'
FsPath = 'demo'
FsOffset = '0x310000'
FsSize = '0XF0000'
```

- [pkg]节点用于生成固件包
- 每种soc都有一个单独的节点，如[esp32]  [esp32c3]
- 用户可自定义部分为`COM` `Baud`  `Firmware` `FsPath`
- `FsOffset` `FsSzize`非必要情况请保持默认

#### 上手指南

1. 安装Python3.8
2. 切换到仓库文件夹，在命令行执行`pip install -r requirements.txt`
3. 修改`config.toml`中的`COM` `Baud`  `Firmware` `FsPath`为您自己的参数
4. 执行`python esp32v2.py -h`查看帮助信息，根据您的需求进行使用

```shell
usage: esp32v2.py [-h] [-t TARGET] [-f] [-r] [-p]

LuatOS-SoC For ESP32 Flash Tool

optional arguments:
  -h, --help            show this help message and exit
  -t TARGET, --target TARGET
                        Chip型号:esp32,es32c3,esp32s2,esp32s3
  -f, --fs              下载脚本
  -r, --rom             下载底层固件
  -p, --pkg             打包固件
```

## 2、鸣谢

排名不分前后

- [LuatOS](https://gitee.com/openLuat/LuatOS) ：合宙LuatOS是运行在嵌入式硬件的实时操作系统,用户编写lua代码就可完成各种功能

- [ESPRESSIF](https://www.espressif.com/): 乐鑫科技，提供高性价比高可玩性的ESP32系列产品

- [Wendal](https://gitee.com/wendal) ：技术大佬，LuatOS领头人

- [Jetbrains](https://www.jetbrains.com/) ：提供Pycharm高效率开发IDE

  

