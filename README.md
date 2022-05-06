# LuatOS For ESP32

[![esp32c3](https://github.com/dreamcmi/LuatOS-ESP32/actions/workflows/commit-build-c3.yml/badge.svg?branch=master)](https://github.com/dreamcmi/LuatOS-ESP32/actions/workflows/commit-build-c3.yml)[![esp32s3](https://github.com/dreamcmi/LuatOS-ESP32/actions/workflows/commit-build-s3.yml/badge.svg?branch=master)](https://github.com/dreamcmi/LuatOS-ESP32/actions/workflows/commit-build-s3.yml)

## 1、简介

尝试给ESP32适配一个[LuatOS](https://gitee.com/openLuat/LuatOS)，只是为了学习，个人项目，希望有更多的朋友来一起贡献。

目前C3、S3基于 ESP-IDFV4.4.1

当前分支ESP32C3/ESP32S3，为主力版本。

本项目仓库只有两个： [Gitee](https://gitee.com/dreamcmi/LuatOS-ESP32)  [Github](https://github.com/dreamcmi/LuatOS-ESP32) 

如果你在其他仓库发现本仓库代码均为非本人授权许可。

##### 项目目录结构:

```
├─components		(组件库)
│  ├─lua			(lua核心代码)
│  ├─luat			(luat移植层)
│  │  ├─freertos	(freertos移植层)
│  │  ├─include		(用户头文件目录)
│  │  └─port		(接口移植目录)
│  ├─led_strip		(ws2812库)
│  ├─libhelix-mp3	(mp3解码库)
│  ├─miniz			(压缩库)
├─demo				(lua示例)
├─doc				(开发相关文档)
├─img				(介绍用相关图片)
├─main				(c程序主入口)
├─patches			(补丁目录)
├─soc_tools			(用于打包luatos-soc的相关文件)
├─test				(测试部测试代码)
├─tools				(工具目录，包括下载打包脚本)
└─userdoc			(用户文档)
```

##### 快速编译上手：

1. 参考乐鑫文档安装ESP-IDF[文档链接](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32c3/get-started/index.html) ,国内用户可以选择Gitee上的[镜像库](https://gitee.com/EspressifSystems/esp-idf) 进行安装。
2. 克隆本仓库和[luatos主仓库](https://gitee.com/openLuat/LuatOS) 到同级目录下
3. 进入LuatOS-ESP32仓库 执行`idf.py set-target esp32c3` `idf.py build`进行编译

```
注：
1、如需打包soc文件，请进入tools目录查看Readme了解。
2、下载脚本建议使用tools中的esp32v3.py脚本操作，当然你也可以选择luatools。
```

## 2、参与方式

使用VisualStudio Code [UserGuide-VSC](./doc/userguide.md)

使用Clion [UserGuide-Clion](./doc/UserGuideCilon.md)

欢迎提issue和pr帮助项目进步。

ESP32开发QQ群：667220396

LuatOS开发QQ群：1061642968

## 3、相关文档

[API-DOC](./apidoc)

[Support](./doc/SUPPORT.md)

[ESP32C3解锁GPIO11](./doc/VDD_SPI_AS_GPIO.md)

进度更新中，欢迎提需求。

## 4、鸣谢

排名不分前后

- [LuatOS](https://gitee.com/openLuat/LuatOS) ：合宙LuatOS是运行在嵌入式硬件的实时操作系统,用户编写lua代码就可完成各种功能

- [ESPRESSIF ](https://www.espressif.com/): 乐鑫科技，提供高性价比高可玩性的ESP32系列产品

- [Wendal](https://gitee.com/wendal) ：技术大佬，LuatOS领头人

## 5、LICENSE

本项目遵循 Apache License 2.0 开源。

