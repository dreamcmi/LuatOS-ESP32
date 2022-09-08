/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LUAT_CONF_BSP
#define LUAT_CONF_BSP
#define LUAT_BSP_VERSION "V0004-BETA"

// ESP32系列开发中标识
#define ESP_BETA_VERSION 1

#define LUAT_MODULE_SEARCH_PATH   "/spiffs/%s.luac", "/spiffs/%s.lua",\
  "/luadb/%s.luac", "/luadb/%s.lua",\
  "/%s.luac", "/%s.lua",\
  "",

//----------------------------------
// 使用Luat VFS(虚拟文件系统)和内置库文件, 与esp32的vfs不冲突
#define LUAT_USE_FS_VFS 1
#define LUAT_USE_VFS_INLINE_LIB 1
#define LUAT_COMPILER_NOWEAK 1
//----------------------------------

//----------------------------
// 外设,按需启用, 最起码启用uart
#define LUAT_USE_UART 1
#define LUAT_USE_GPIO 1
#define LUAT_USE_I2C  1
#define LUAT_USE_SPI  1
// #define LUAT_USE_TWAI 1
#define LUAT_USE_I2S 1
#define LUAT_USE_ADC  1
#define LUAT_USE_PWM  1
#define LUAT_USE_PWM2  1
#define LUAT_USE_RMT 1
#define LUAT_USE_WDT 1

#define LUAT_USE_WLAN 1
#define LUAT_USE_ESPNOW 1
#define LUAT_USE_ESP32LIB 1
#define LUAT_USE_ESP32_SDMMC 1
#define LUAT_USE_SOCKET 1
#define LUAT_USE_NTP 1
#define LUAT_USE_BLE 1
#define LUAT_USE_ESPHTTP 1
#define LUAT_USE_ESPMQTT 1
#define LUAT_USE_ESPWEBSOCKET 1
#define LUAT_USE_MCU  1
#define LUAT_USE_RTC 1
// #define LUAT_USE_ESPSSL 1

//----------------------------
// 常用工具库, 按需启用, cjson和pack是强烈推荐启用的
#define LUAT_USE_CRYPTO  1
#define LUAT_USE_CJSON  1
#define LUAT_USE_ZBUFF  1
#define LUAT_USE_PACK  1
#define LUAT_USE_LIBGNSS  1
#define LUAT_USE_FS  1
#define LUAT_USE_SENSOR  1
#define LUAT_USE_SFUD  1
#define LUAT_USE_MINIZ 1
// #define LUAT_USE_LORA 1
// #define LUAT_USE_MLX90640 1
#define LUAT_USE_IOTAUTH 1
// #define LUAT_USE_PROTOBUF

// 性能测试
// #define LUAT_USE_COREMARK 1
// #define LUAT_USE_FATFS 1

//----------------------------
// 高通字体, 需配合芯片使用
// #define LUAT_USE_GTFONT 1
// #define LUAT_USE_GTFONT_UTF8

//----------------------------
// 高级功能, 其中shell是推荐启用, 除非你打算uart0也读数据
#define LUAT_USE_SHELL 1
// 高级功能, debug
#define LUAT_USE_DBG 1
// FDB 提供kv数据库, 与nvm库类似
#define LUAT_USE_FDB 1
#define LUAT_USE_OTA
#define UPDATE_BIN_PATH "/spiffs/update.bin"

//---------------------
// UI
// LCD  是彩屏, 若使用LVGL就必须启用LCD
#define LUAT_USE_LCD
#define LUAT_USE_TJPGD
// EINK 是墨水屏
#define LUAT_USE_EINK

//---------------------
// U8G2
// 单色屏, 支持i2c/spi
#define LUAT_USE_DISP
#define LUAT_USE_U8G2

/**************FONT*****************/
#define LUAT_USE_FONTS
/**********U8G2&LCD FONT*************/
#define USE_U8G2_OPPOSANSM_ENGLISH 1
// #define USE_U8G2_OPPOSANSM8_CHINESE
// #define USE_U8G2_OPPOSANSM10_CHINESE
#define USE_U8G2_OPPOSANSM12_CHINESE
// #define USE_U8G2_OPPOSANSM16_CHINESE
// #define USE_U8G2_OPPOSANSM24_CHINESE
/**********LVGL FONT*************/
// #define LV_FONT_OPPOSANS_M_8
// #define LV_FONT_OPPOSANS_M_10
// #define LV_FONT_OPPOSANS_M_12
// #define LV_FONT_OPPOSANS_M_16


// ---------------------
// LVGL
// 主推的UI库, 功能强大但API繁琐
#define LUAT_USE_LVGL
#define LV_DISP_DEF_REFR_PERIOD 30
#define LUAT_LV_DEBUG 0

#define LV_MEM_CUSTOM 1

#ifdef LUAT_USE_LVGL
#ifdef LUAT_USE_TJPGD
#undef LUAT_USE_TJPGD
#endif
#endif

#define LUAT_USE_LVGL_INDEV 1 // 输入设备
// #define LV_USE_LOG 1

#define LUAT_USE_LVGL_ARC   //圆弧 无依赖
#define LUAT_USE_LVGL_BAR   //进度条 无依赖
#define LUAT_USE_LVGL_BTN   //按钮 依赖容器CONT
#define LUAT_USE_LVGL_BTNMATRIX   //按钮矩阵 无依赖
#define LUAT_USE_LVGL_CALENDAR   //日历 无依赖
#define LUAT_USE_LVGL_CANVAS   //画布 依赖图片IMG
#define LUAT_USE_LVGL_CHECKBOX   //复选框 依赖按钮BTN 标签LABEL
#define LUAT_USE_LVGL_CHART   //图表 无依赖
#define LUAT_USE_LVGL_CONT   //容器 无依赖
#define LUAT_USE_LVGL_CPICKER   //颜色选择器 无依赖
#define LUAT_USE_LVGL_DROPDOWN   //下拉列表 依赖页面PAGE 标签LABEL
#define LUAT_USE_LVGL_GAUGE   //仪表 依赖进度条BAR 仪表(弧形刻度)LINEMETER
#define LUAT_USE_LVGL_IMG   //图片 依赖标签LABEL
#define LUAT_USE_LVGL_IMGBTN   //图片按钮 依赖按钮BTN
#define LUAT_USE_LVGL_KEYBOARD   //键盘 依赖图片按钮IMGBTN
#define LUAT_USE_LVGL_LABEL   //标签 无依赖
#define LUAT_USE_LVGL_LED   //LED 无依赖
#define LUAT_USE_LVGL_LINE   //线 无依赖
#define LUAT_USE_LVGL_LIST   //列表 依赖页面PAGE 按钮BTN 标签LABEL
#define LUAT_USE_LVGL_LINEMETER   //仪表(弧形刻度) 无依赖
#define LUAT_USE_LVGL_OBJMASK   //对象蒙版 无依赖
#define LUAT_USE_LVGL_MSGBOX   //消息框 依赖图片按钮IMGBTN 标签LABEL
#define LUAT_USE_LVGL_PAGE   //页面 依赖容器CONT
#define LUAT_USE_LVGL_SPINNER   //旋转器 依赖圆弧ARC 动画ANIM
#define LUAT_USE_LVGL_ROLLER   //滚筒 无依赖
#define LUAT_USE_LVGL_SLIDER   //滑杆 依赖进度条BAR
#define LUAT_USE_LVGL_SPINBOX   //数字调整框 无依赖
#define LUAT_USE_LVGL_SWITCH   //开关 依赖滑杆SLIDER
#define LUAT_USE_LVGL_TEXTAREA   //文本框 依赖标签LABEL 页面PAGE
#define LUAT_USE_LVGL_TABLE   //表格 依赖标签LABEL
#define LUAT_USE_LVGL_TABVIEW   //页签 依赖页面PAGE 图片按钮IMGBTN
#define LUAT_USE_LVGL_TILEVIEW   //平铺视图 依赖页面PAGE
#define LUAT_USE_LVGL_WIN   //窗口 依赖容器CONT 按钮BTN 标签LABEL 图片IMG 页面PAGE

#define LV_HOR_RES_MAX          (320)
#define LV_VER_RES_MAX          (320)
#define LV_COLOR_DEPTH          16

#define LV_COLOR_16_SWAP   1

// #define LV_TICK_CUSTOM 1
// #define LV_TICK_CUSTOM_INCLUDE  "freertos/task.h"         /*Header for the system time function*/
// #define LV_TICK_CUSTOM_SYS_TIME_EXPR ((uint32_t)xTaskGetTickCount())     /*Expression evaluating to current system time in ms*/

#endif
