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
