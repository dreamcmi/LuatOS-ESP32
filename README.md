# LuatOS For ESP32

### 1、简介

尝试给ESP32适配一个[LuatOS](https://gitee.com/openLuat/LuatOS)，只是为了学习。希望有更多的朋友来一起贡献。

目前基于ESP-IDF 4.2 release版本

### 2、进度

##### 2021.4.10

点亮了。。。仅此而已

##### 2020.4.11

1、spiffs文件系统做好了，可以刷入lua脚本

2、似乎idf的freertos有所不同，sys.wait会发生cpu_reset，具体原因还在查找

##### 2020.4.13

还在查bug，这几天先把简单外设做了

##### 2020.4.14

1、sys.wait问题已解决，感谢wendal大佬的帮助

2、添加gpio库，可以进行gpio操作了

##### 2020.4.15

1、adc基础搭好了，close和返回电压暂时搁置。

2、同步更新LuatOS上游源码

### 3、参与方式

1、vscode安装espidf插件

2、git clone https://gitee.com/dreamcmi/LuatOS-ESP32 

3、用vscode打开文件夹，添加/修改/删除

4、提个pr呗