#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_msgbus.h"
#include "luat_log.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/ets_sys.h"
#endif
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

LUAMOD_API int luaopen_espnow(lua_State* L);
LUAMOD_API int luaopen_rmt(lua_State* L);
LUAMOD_API int luaopen_esp32(lua_State* L);
LUAMOD_API int luaopen_espnow(lua_State* L);
LUAMOD_API int luaopen_pwm2(lua_State* L);

static const luaL_Reg loadedlibs[] = {
    {"_G", luaopen_base},               // _G
    {LUA_LOADLIBNAME, luaopen_package}, // require
    {LUA_COLIBNAME, luaopen_coroutine}, // coroutine协程库
    {LUA_TABLIBNAME, luaopen_table},    // table库,操作table类型的数据结构
    {LUA_IOLIBNAME, luaopen_io},        // io库,操作文件
    {LUA_OSLIBNAME, luaopen_os},        // os库,已精简
    {LUA_STRLIBNAME, luaopen_string},   // string库,字符串操作
    {LUA_MATHLIBNAME, luaopen_math},    // math 数值计算
    {LUA_DBLIBNAME, luaopen_debug},     // debug库,已精简
#if defined(LUA_COMPAT_BITLIB)
    {LUA_BITLIBNAME, luaopen_bit32}, // 不太可能启用
#endif

//-----------------------------------------------------------------------
// 设备驱动类, 可按实际情况删减. 即使最精简的固件, 也强烈建议保留uart库
#ifdef LUAT_USE_UART
  {"uart",    luaopen_uart},              // 串口操作
#endif
#ifdef LUAT_USE_GPIO
  {"gpio",    luaopen_gpio},              // GPIO脚的操作
#endif
#ifdef LUAT_USE_I2C
  {"i2c",     luaopen_i2c},               // I2C操作
#endif
#ifdef LUAT_USE_SPI
  {"spi",     luaopen_spi},               // SPI操作
#endif
#ifdef LUAT_USE_ADC
  {"adc",     luaopen_adc},               // ADC模块
#endif
#ifdef LUAT_USE_SDIO
  {"sdio",     luaopen_sdio},             // SDIO模块
#endif
#ifdef LUAT_USE_PWM
  {"pwm",     luaopen_pwm},               // PWM模块
#endif
#ifdef LUAT_USE_WDT
  {"wdt",     luaopen_wdt},               // watchdog模块
#endif
#ifdef LUAT_USE_PM
  {"pm",      luaopen_pm},                // 电源管理模块
#endif
#ifdef LUAT_USE_MCU
  {"mcu",     luaopen_mcu},               // MCU特有的一些操作
#endif
#ifdef LUAT_USE_HWTIMER
  {"hwtimer", luaopen_hwtimer},           // 硬件定时器
#endif
#ifdef LUAT_USE_RTC
  {"rtc", luaopen_rtc},                   // 实时时钟
#endif
//-----------------------------------------------------------------------
// 工具库, 按需选用
#ifdef LUAT_USE_CRYPTO
  {"crypto",luaopen_crypto},            // 加密和hash模块
#endif
#ifdef LUAT_USE_CJSON
  {"json",    luaopen_cjson},          // json的序列化和反序列化
#endif
#ifdef LUAT_USE_ZBUFF
  {"zbuff",   luaopen_zbuff},             // 像C语言语言操作内存块
#endif
#ifdef LUAT_USE_PACK
  {"pack",    luaopen_pack},              // pack.pack/pack.unpack
#endif
  // {"mqttcore",luaopen_mqttcore},          // MQTT 协议封装
  // {"libcoap", luaopen_libcoap},           // 处理COAP消息

#ifdef LUAT_USE_GNSS
  {"libgnss", luaopen_libgnss},           // 处理GNSS定位数据
#endif
#ifdef LUAT_USE_FS
  {"fs",      luaopen_fs},                // 文件系统库,在io库之外再提供一些方法
#endif
#ifdef LUAT_USE_SENSOR
  {"sensor",  luaopen_sensor},            // 传感器库,支持DS18B20
#endif
#ifdef LUAT_USE_SFUD
  {"sfud", luaopen_sfud},              // sfud
#endif
#ifdef LUAT_USE_DISP
  {"disp",  luaopen_disp},              // OLED显示模块,支持SSD1306
#endif
#ifdef LUAT_USE_U8G2
  {"u8g2", luaopen_u8g2},              // u8g2
#endif

#ifdef LUAT_USE_EINK
  {"eink",  luaopen_eink},              // 电子墨水屏,试验阶段
#endif

#ifdef LUAT_USE_LVGL
#ifndef LUAT_USE_LCD
#define LUAT_USE_LCD
#endif
  {"lvgl",   luaopen_lvgl},
#endif

#ifdef LUAT_USE_LCD
  {"lcd",    luaopen_lcd},
#endif
#ifdef LUAT_USE_STATEM
  {"statem",    luaopen_statem},
#endif
#ifdef LUAT_USE_GTFONT
  {"gtfont",    luaopen_gtfont},
#endif
#ifdef LUAT_USE_NIMBLE
  {"nimble",    luaopen_nimble},
#endif
#ifdef LUAT_USE_FDB
  {"fdb",       luaopen_fdb},
#endif
#ifdef LUAT_USE_LCDSEG
  {"lcdseg",       luaopen_lcdseg},
#endif
#ifdef LUAT_USE_VMX
  {"vmx",       luaopen_vmx},
#endif
#ifdef LUAT_USE_COREMARK
  {"coremark", luaopen_coremark},
#endif
    {"wlan", luaopen_wlan},   // wlan/wifi联网操作
    // {"lwip",luaopen_lwip},    // lwip操作
    // {"ble",luaopen_ble},      // ble操作
    {"espnow",luaopen_espnow},// espnow操作
    // {"rmt",luaopen_rmt},      // rmt操作
    {"esp32",luaopen_esp32},  // esp32专用库
    {"pwm2",luaopen_pwm2},    // pwm2
    {"socket",luaopen_socket},// socket
    {NULL, NULL}};

// 按不同的rtconfig加载不同的库函数
void luat_openlibs(lua_State *L)
{
  // 初始化队列服务
  luat_msgbus_init();
  //print_list_mem("done>luat_msgbus_init");
  // 加载系统库
  const luaL_Reg *lib;
  /* "require" functions from 'loadedlibs' and set results to global table */
  for (lib = loadedlibs; lib->func; lib++)
  {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1); /* remove lib */
                   //extern void print_list_mem(const char* name);
                   //print_list_mem(lib->name);
  }
}


// 微妙硬延时
void luat_timer_us_delay(size_t us) {
  if (us > 0)
    ets_delay_us(us);
}

//esp32重启函数
void luat_os_reboot(int code)
{
  esp_restart();
}

const char *luat_os_bsp(void)
{
#if CONFIG_IDF_TARGET_ESP32C3
  return "ESP32C3";
#elif CONFIG_IDF_TARGET_ESP32S3
  return "ESP32S3";
#endif
}

void luat_os_standy(int timeout)
{
}

void luat_os_entry_cri(void)
{
  vPortEnterCritical();
}

void luat_os_exit_cri(void)
{
  vPortExitCritical();
}

void luat_meminfo_sys(size_t *total, size_t *used, size_t *max_used)
{
  *used = esp_get_free_heap_size();
  *max_used = esp_get_free_heap_size();
  *total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
}

