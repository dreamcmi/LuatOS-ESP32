# 制作脚本刷写文件

1、生成个bin

```
mkspiffs -c [src_folder] -b 4096 -p 256 -s 0xF0000 demo.bin
```

2、刷入

```
esptool.py --chip esp32 --port [port] --baud [baud] write_flash -z 0x310000 spiffs.bin
```