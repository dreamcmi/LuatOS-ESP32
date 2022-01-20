# CHANGELOG

## 2021.10.31

#### ESP32C3 V0001_BETA发布 

```c
// 支持列表
{"rtos", luaopen_rtos}, 	// rtos底层库, 核心功能是队列和定时器
{"log", luaopen_log}, 		// 日志库
{"timer", luaopen_timer}, 	// 延时库
{"crypto", luaopen_crypto},	// 加密和hash库
{"json", luaopen_cjson},   	// json的序列化和反序列化
{"zbuff",luaopen_zbuff},  	// zbuff
{"gpio", luaopen_gpio},   	// GPIO脚的操作
{"adc", luaopen_adc}, 		// ADC库
{"i2c", luaopen_i2c}, 		// I2C操作
{"spi", luaopen_spi}, 		// SPI操作
{"uart",luaopen_uart},		// UART操作
{"pwm",luaopen_pwm},  		// PWM操作
{"wlan", luaopen_wlan},   	// wlan/wifi联网操作
{"lwip",luaopen_lwip},		// lwip操作
{"esp32",luaopen_esp32},  	// esp32专用库
```

下载工具：[esp32-flashtool](https://gitee.com/dreamcmi/esp32-flashtool)

FS分区：960k

LUAVM: 64k

```
IO对照表(C3):
MISO -- 6
MOSI -- 7
CLK  -- 8

SCL  -- 2
SDA  -- 3

TX1  -- 4
RX1  -- 5

ADC  -- 0 1 2 3 4

PWM  -- 0 1
```

欢迎大家试用，有问题提issue即可。

## 2021.11.13

#### ESP32 V0001_BETA2

Fix:

1. I2C id分配错误

2. SPI id分配错误

3. uart1引脚分配错误

4. 关闭mbedtls本地验证

5. tools依赖不全

Add:

1. esp32专用库(mac,rstReason,random,chip,wakeupcause,lightsleep,deepsleep)
2. esp32s3初步支持
3. pwm2库，更贴近idf的ledc使用
4. 部分demo供参考

Update：

1. 刷机工具更新到v3，固件格式修改为bin
2. 4M版本分区表取消otadata
3. 底层同步更新上游V0007

注：

1. 本次同时发布ESP32C3和ESP32S3固件，C3使用idf4.3.1，S3使用release/v4.4。
2. S3目前上游并未release，可能有较多问题。
3. 固件格式改为bin，用户可直接使用esptool刷写，偏移地址0x0。
4. 刷写脚本请使用esp32v3.py工具，不兼容v2，默认开启luac。
5. 网络部分(wifi,ble,espnow,lwip,mbedtls)将在后续版本陆续完善。
6. 当您发现固件有问题时，请在仓库提交issue并等待开发者回复，我们同时欢迎您自行编译固件或者提交pr。

## 2021.11.25

#### ESP32 V0001_BETA3

Fix:

1. ESPNOW 初始化和去初始化参数不完整

Add:

1. ESPNOW基础功能实现

2. socket基础功能实现

3. ESP32S3启用psram



注：ESP32S3发布固件分为两种，主要为了区分psram的接口类型，未接psram的可随意选择无影响。

## 2021.12.24

### ESP32 V0001_Beta4

Fix: 

1. SPI收发
2. wifi连接逻辑优化

Add:

1. LCD库
2. U8G2
3. LVGL7
4. Luadb文件系统
5. esphttp库
6. 分区表完善4/8/16M
7. esp32库增加温度获取(C3)
8. NTP库
9. SFUD库
10. pinmap.h 引脚统一配置

Update:

1. 固件格式统一luatos-soc格式,esp32v3工具更新支持,默认脚本路径为`/luadb/`
2. idf版本更新到4.3.2(C3)
3. 规范部分底层log
4. 源码依赖LuatOS主仓库,本仓库只包含接口文件和工具

## 2022.1.20

### ESP32 V0001-RC1

Fix:

1. GPIO中断
2. UART中断
3. socket阻塞问题
4. 打包格式文件名错误
5. TASK_WDT_TIMEOUT改为10s
6. i2c多字节读取数据错乱
7. uart读取阻塞问题
8. C3 io18和19不可用问题

Add:

1. ESPHTTP
2. ESPMQTT
3. PWM(仅做兼容)
4. socket支持dns解析域名
5. 启用libgnss
6. uart0开放使用,115200 8n1
7. smartconfig
8. 开机打印mac,便于分辨模组
9. C3支持获取芯片温度

Update:

1. esp-idf更新到v4.4

Feature:

1. 支持调试,预计正式版加入
2. BLE,预计v0002或v0003可用
