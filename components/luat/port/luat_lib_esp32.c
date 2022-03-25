/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  esp32
@summary esp32专用库
@version 1.0
@date    2022.2.15
*/
#include "luat_base.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "driver/temp_sensor.h"

#include <string.h>

/*
获取mac
@api esp32.getmac(id)
@int mac地址来源 0:ESP_MAC_WIFI_STA 1:ESP_MAC_WIFI_SOFTAP 2:ESP_MAC_BT 3:ESP_MAC_ETH
@return string mac地址
@usage
esp32.getmac(0)
*/
static int l_esp32_getmac(lua_State *L)
{
    int type = luaL_optinteger(L, 1, 0);
    uint8_t *mac = malloc(10);
    esp_read_mac(mac, type);
    lua_pushlstring(L, (const char *)mac, 6);
    free(mac);
    return 1;
}

/*
获取重启原因
@api esp32.getRstReason()
@return int esp_reset_reason_t
@usage
esp32.getRstReason()
*/
static int l_esp32_get_rst_reason(lua_State *L)
{
    esp_reset_reason_t reason = esp_reset_reason();
    lua_pushinteger(L, (int)reason);
    return 1;
}

/*
获取随机数
@api esp32.random()
@return int 随机数
@usage
r = esp32.random()
*/
static int l_esp32_random(lua_State *L)
{
    uint32_t r = esp_random();
    lua_pushinteger(L, r);
    return 1;
}

/*
获取chip信息
@api esp32.getchip()
@return table chip信息表
@usage
local re = esp32.getchip()
log.info("esp32", "chip", re['chip'])
log.info("esp32", "features", re['features'])
log.info("esp32", "cores", re['cores'])
log.info("esp32", "revision", re['revision'])
*/
static int l_esp32_get_chip(lua_State *L)
{
    esp_chip_info_t info = {0};
    esp_chip_info(&info);

    lua_newtable(L);

    lua_pushstring(L, "chip");
    lua_pushinteger(L, info.model);
    lua_settable(L, -3);

    lua_pushstring(L, "features");
    lua_pushinteger(L, info.features);
    lua_settable(L, -3);

    lua_pushstring(L, "cores");
    lua_pushinteger(L, info.cores);
    lua_settable(L, -3);

    lua_pushstring(L, "revision");
    lua_pushinteger(L, info.revision);
    lua_settable(L, -3);

    return 1;
}

/*
获取唤醒原因
@api esp32.getWakeupCause()
@return int esp_sleep_wakeup_cause_t
@usage
cause = esp32.getWakeupCause()
*/
static int l_esp32_get_wakeup_cause(lua_State *L)
{
    esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
    lua_pushinteger(L, (int)reason);
    return 1;
}

/*
lightsleep
@api esp32.enterLightSleep(waketype,pin/rtc,level/nil)
@int 唤醒类型 gpio(0) rtc(1)
@int 唤醒条件 gpio(pin) rtc(time:us)
@int gpio唤醒沿 0:下降沿 1:上升沿
@usage
-- gpio唤醒
esp32.enterLightSleep(0,9,0)
-- rtc唤醒
esp32.enterLightSleep(1,10*1000*1000)
*/

static int l_esp32_enter_light_sleep(lua_State *L)
{
    int waketype = luaL_checkinteger(L, 1);
    if (waketype == 0)
    {
        int pin = luaL_checkinteger(L, 2);
        int level = luaL_checkinteger(L, 3);
        if (!esp_sleep_is_valid_wakeup_gpio(pin))
        {
            ESP_LOGE("LPM", "Error wakeup pin:%d", pin);
            return 0;
        }
        gpio_reset_pin(pin);
        gpio_config_t config = {
            .pin_bit_mask = BIT64(pin),
            .mode = GPIO_MODE_INPUT};
        gpio_config(&config); // todo error check
        gpio_wakeup_enable(pin, level == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
        esp_sleep_enable_gpio_wakeup();
        uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
        esp_light_sleep_start();
    }
    else if (waketype == 1)
    {
        int time = luaL_checkinteger(L, 2);
        esp_sleep_enable_timer_wakeup(time);
        uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
        esp_light_sleep_start();
    }
    else
    {
        ESP_LOGE("LPM", "Error lightsleep type:%d", waketype);
    }
    return 0;
}

/*
deepsleep
@api esp32.enterDeepSleep(waketype,pin/rtc,level/nil)
@int 唤醒类型 gpio(0) rtc(1)
@int 唤醒条件 gpio(pin) rtc(time:us)
@int gpio唤醒沿 0:下降沿 1:上升沿
@usage
-- gpio唤醒
esp32.enterDeepSleep(0,9,0)
-- rtc唤醒
esp32.enterDeepSleep(1,10*1000*1000)
*/

static int l_esp32_enter_deep_sleep(lua_State *L)
{
    int waketype = luaL_checkinteger(L, 1);
    if (waketype == 0)
    {
        int pin = luaL_checkinteger(L, 2);
        int level = luaL_checkinteger(L, 3);
        gpio_reset_pin(pin);
#if CONFIG_IDF_TARGET_ESP32C3
        gpio_config_t config = {
            .pin_bit_mask = BIT(pin),
            .mode = GPIO_MODE_INPUT};
        gpio_config(&config);                               // todo error check
        esp_deep_sleep_enable_gpio_wakeup(BIT(pin), level); // todo error check
#else
        const uint64_t ext_wakeup_pin_mask = 1ULL << pin;
        esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_mask, level);
#endif
        uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
        esp_deep_sleep_start();
    }
    else if (waketype == 1)
    {
        int time = luaL_checkinteger(L, 2);
        esp_sleep_enable_timer_wakeup(time);
        uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
        esp_deep_sleep_start();
    }
    else
    {
        ESP_LOGE("LPM", "Error lightsleep type:%d", waketype);
    }
    return 0;
}

/*
esp_err转文字
@api esp32.errToName()
@int esp_err
@return string
@usage
log.info("esp32.err_name",esp32.errToName(0))
*/
static int l_esp32_error_to_name(lua_State *L)
{
    esp_err_t err = (esp_err_t)luaL_checkinteger(L, 1);
    lua_pushstring(L, esp_err_to_name(err));
    return 1;
}

#if CONFIG_IDF_TARGET_ESP32C3
/*
esp32c3芯片温度
@api esp32.temp()
@return float temp
@usage
log.info("esp32","temp",esp32.temp())
*/
static int l_esp32_temp(lua_State *L)
{
    float tsens_out = 0;
    temp_sensor_start();
    temp_sensor_read_celsius(&tsens_out);
    temp_sensor_stop();
    lua_pushnumber(L, tsens_out);
    return 1;
}
#endif

#include "rotable.h"
static const rotable_Reg reg_esp32[] =
    {
        {"getmac", l_esp32_getmac, 0},
        {"getRstReason", l_esp32_get_rst_reason, 0},
        {"random", l_esp32_random, 0},
        {"getchip", l_esp32_get_chip, 0},
        {"getWakeupCause", l_esp32_get_wakeup_cause, 0},
        {"enterLightSleep", l_esp32_enter_light_sleep, 0},
        {"enterDeepSleep", l_esp32_enter_deep_sleep, 0},
        {"errToName", l_esp32_error_to_name, 0},
#if CONFIG_IDF_TARGET_ESP32C3
        {"temp", l_esp32_temp, 0},
#endif
        {"GPIO", NULL, 0},
        {"RTC", NULL, 1},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_esp32(lua_State *L)
{
#if CONFIG_IDF_TARGET_ESP32C3
    temp_sensor_config_t temp_sensor = {0};
    temp_sensor.dac_offset = TSENS_DAC_L2;
    temp_sensor.clk_div = 6;
    temp_sensor_set_config(temp_sensor);
#endif
    luat_newlib(L, reg_esp32);
    return 1;
}
