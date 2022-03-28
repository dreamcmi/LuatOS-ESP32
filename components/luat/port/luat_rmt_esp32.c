/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  rmt
@summary esp专用rmt库
@version 1.0
@date    2022.2.15
*/
#include "luat_base.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "led_strip.h"

/*
初始化rmt
@api rmt.init(io,ch,div)
@int 配置GPIO
@int 配置rmt通道
@int 驱动方向 rmt.TX rmt.RX
@int apb分频参数(APB时钟80Mhz)
@return int  esp_err 成功0
@usage
rmt.init(8,0,rmt.TX,2)
*/
static int l_rmt_init(lua_State *L)
{
    int rmtio = luaL_checkinteger(L, 1);
    int rmtch = luaL_checkinteger(L, 2);
    int dir = luaL_optinteger(L, 3, 0);
    int rmtdiv = luaL_optinteger(L, 4, 80);
    rmt_config_t config = {0};
    config.channel = rmtch;
    config.gpio_num = rmtio;
    config.mem_block_num = 1;
    config.flags = 0;
    config.clk_div = (uint8_t)rmtdiv;

    switch (dir)
    {
    case 0:
        config.rmt_mode = RMT_MODE_TX;
        config.tx_config.carrier_freq_hz = 38000;
        config.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
        config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
        config.tx_config.carrier_duty_percent = 33;
        config.tx_config.carrier_en = false;
        config.tx_config.loop_en = false;
        config.tx_config.idle_output_en = true;
        break;
    case 1:
        config.rmt_mode = RMT_MODE_RX;
        config.rx_config.idle_threshold = 12000;
        config.rx_config.filter_ticks_thresh = 100;
        config.rx_config.filter_en = true;
        break;
    default:
        lua_pushinteger(L, ESP_ERR_INVALID_ARG);
        return 1;
        break;
    }

    rmt_config(&config);
    esp_err_t err = rmt_driver_install(config.channel, 0, 0);
    lua_pushinteger(L, err);
    return 1;
}

/*
去初始化rmt
@api rmt.deinit(ch)
@int 配置rmt通道
@return int  esp_err 成功0
@usage
rmt.deinit(0)
*/
static int l_rmt_deinit(lua_State *L)
{
    esp_err_t err = rmt_driver_uninstall(luaL_checkinteger(L, 1));
    lua_pushinteger(L, err);
    return 1;
}

/*
初始化ws2812
@api rmt.ws2812Init(num,channel)
@int num 灯珠数量
@int channel rmt tx通道,需要先初始化rmt
@return userdata 如果成功返回handle,失败返回nil
@usage
handle = rmt.ws2812Init(10, 0)
assert(handle ~= nil, "ws2812.init error")
*/
static int l_rmt_ws2812_init(lua_State *L)
{
    int num = luaL_checkinteger(L, 1);
    int channel = luaL_checkinteger(L, 2);
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(num, (led_strip_dev_t)channel);
    led_strip_t *strip = led_strip_new_rmt_ws2812(&strip_config);
    if (strip != NULL)
    {
        lua_pushlightuserdata(L, (void *)strip);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

/*
ws2812清空显示
@api rmt.ws2812Clear(handle)
@userdata 通过rmt.ws2812Init返回的handle
@int 操作超时时间,可选,默认1000ms
@return esp_err_t
@usage
log.info("ws2812.clear", rmt.ws2812Clear(handle))
*/
static int l_rmt_ws2812_clear(lua_State *L)
{
    led_strip_t *strip = (led_strip_t *)lua_touserdata(L, 1);
    int timeout = luaL_optinteger(L, 2, 1000);
    esp_err_t err = strip->clear(strip, timeout);
    lua_pushinteger(L, err);
    return 1;
}

/*
ws2812设置颜色
@api rmt.ws2812SetPixel(handle, num, r, g, b)
@userdata 通过rmt.ws2812Init返回的handle
@int 要操作的灯珠编号,注意从0开始
@int red 0-255
@int green 0-255
@int blue 0-255
@return esp_err_t
@usage
log.info("ws2812.setpixel", rmt.ws2812SetPixel(handle, 0, 255, 0, 0))
*/
static int l_rmt_ws2812_set_pixel(lua_State *L)
{
    led_strip_t *strip = (led_strip_t *)lua_touserdata(L, 1);
    int num = luaL_checkinteger(L, 2);
    int red = luaL_checkinteger(L, 3);
    int green = luaL_checkinteger(L, 4);
    int blue = luaL_checkinteger(L, 5);
    esp_err_t err = strip->set_pixel(strip, num, red, green, blue);
    lua_pushinteger(L, err);
    return 1;
}

/*
ws2812刷新
@api rmt.ws2812Refresh(handle)
@userdata 通过rmt.ws2812Init返回的handle
@int 操作超时时间,可选,默认1000ms
@return esp_err_t
@usage
log.info("ws2812.refresh", rmt.ws2812Refresh(handle))
*/
static int l_rmt_ws2812_refresh(lua_State *L)
{
    led_strip_t *strip = (led_strip_t *)lua_touserdata(L, 1);
    int timeout = luaL_optinteger(L, 2, 1000);
    esp_err_t err = strip->refresh(strip, timeout);
    lua_pushinteger(L, err);
    return 1;
}

/*
ws2812去初始化
@api rmt.ws2812Deinit(handle)
@userdata 通过rmt.ws2812Init返回的handle
@return esp_err_t
@usage
log.info("ws2812.deinit", rmt.ws2812Deinit(handle))
*/
static int l_rmt_ws2812_deinit(lua_State *L)
{
    led_strip_t *strip = (led_strip_t *)lua_touserdata(L, 1);
    esp_err_t err = strip->del(strip);
    lua_pushinteger(L, err);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_rmt[] =
    {
        {"init", l_rmt_init, 0},
        {"deinit", l_rmt_deinit, 0},
        {"ws2812Init", l_rmt_ws2812_init, 0},
        {"ws2812Clear", l_rmt_ws2812_clear, 0},
        {"ws2812SetPixel", l_rmt_ws2812_set_pixel, 0},
        {"ws2812Refresh", l_rmt_ws2812_refresh, 0},
        {"ws2812Deinit", l_rmt_ws2812_deinit, 0},

        {"TX", NULL, 0},
        {"RX", NULL, 1},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_rmt(lua_State *L)
{
    luat_newlib(L, reg_rmt);
    return 1;
}
