/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  twai
@summary esp专用库twai
@version 1.0
@date    2022.2.15
*/
#include "luat_base.h"

#include "string.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"

#include "pinmap.h"

/*
初始化twai
@api twai.setup(brp,seg1,seg2,sjw,accept_code,accept_mask,mode)
@int brq 分频参数
@int seg1 时序段1
@int seg2 时序段2
@int sjw 重新同步跳跃宽度
@int accept_code 过滤器接收符 32bit
@int accept_mask 过滤器接收掩码 32bit
@int mode 默认TWAI_MODE_NORMAL 可选(normal:0,no_ack:1,listen_only:2)
@return bool
@usage
twai.setup(4,15,4,3,0,0xffffffff)
*/
static int l_twai_setup(lua_State *L)
{
    esp_err_t err = -1;
    twai_timing_config_t t_config = {
        .brp = luaL_checkinteger(L, 1),
        .tseg_1 = luaL_checkinteger(L, 2),
        .tseg_2 = luaL_checkinteger(L, 3),
        .sjw = luaL_checkinteger(L, 4),
        .triple_sampling = false};
    twai_filter_config_t f_config = {
        .acceptance_code = luaL_checkinteger(L, 5),
        .acceptance_mask = luaL_checkinteger(L, 6),
        .single_filter = true};
#if CONFIG_IDF_TARGET_ESP32C3
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(_C3_TWAI_TX, _C3_TWAI_RX, luaL_optinteger(L, 7, TWAI_MODE_NORMAL));
#elif CONFIG_IDF_TARGET_ESP32S3
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(_S3_TWAI_TX, _S3_TWAI_RX, luaL_optinteger(L, 7, TWAI_MODE_NORMAL));
#else
#error "TWAI SETUP ERROR Please make sure the target is esp32c3 or esp32s3"
#endif
    twai_driver_install(&g_config, &t_config, &f_config); //todo error check
    err = twai_start();
    lua_pushboolean(L, err = ESP_OK ? true : false);
    return 1;
}

/*
关闭twai
@api twai.close
@return bool
@usage
twai.close()
*/
static int l_twai_close(lua_State *L)
{
    esp_err_t err = -1;
    twai_stop(); //todo error check
    err = twai_driver_uninstall();
    lua_pushboolean(L, err = ESP_OK ? true : false);
    return 1;
}

/*
twai发送数据
@api twai.send(id,data)
@int id
@string data
@return bool
@usage
twai.send(0x12,string.Fromhex("1234"))
*/
static int l_twai_send(lua_State *L)
{
    esp_err_t err = -1;
    size_t len = 0;
    twai_message_t message = {0};
    message.identifier = luaL_checkinteger(L, 1);
    const char *data = luaL_checklstring(L, 2, &len);
    message.data_length_code = len;
    memcpy(message.data, data, len);
    err = twai_transmit(&message, portMAX_DELAY);
    lua_pushboolean(L, err = ESP_OK ? true : false);
    return 1;
}

/*
twai接收数据
@api twai.recv(id,len)
@int id
@int len 接收数据长度
@return bool
@usage
twai.recv(0x12,8)
*/
static int l_twai_recv(lua_State *L)
{
    esp_err_t err = -1;
    twai_message_t message = {0};
    err = twai_receive(&message, 100 / portTICK_RATE_MS);
    if (err == ESP_OK)
    {
        if (message.identifier == luaL_checkinteger(L, 1))
        {
            int len = luaL_checkinteger(L, 2);
            lua_pushlstring(L, (const char *)message.data, len);
            return 1;
        }
        else
        {
            lua_pushnil(L);
            return 1;
        }
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
}

/*
twai获取警报
@api twai.getAlerts()
@return int
@usage
local arg = twai.getAlerts()
*/
static int l_twai_get_alerts(lua_State *L)
{
    uint32_t alerts = 0;
    twai_read_alerts(&alerts, 100 / portTICK_RATE_MS);
    lua_pushinteger(L, alerts);
    return 1;
}

/*
twai重新配置启用警报
@api twai.reconfigureAlerts(mask)
@int mask
@return bool
@usage
twai.reconfigureAlerts(0x00002000)
*/
static int l_twai_reconfigure_alerts(lua_State *L)
{
    esp_err_t err = -1;
    int data = luaL_checkinteger(L, 1);
    err = twai_reconfigure_alerts(data, NULL);
    lua_pushboolean(L, err = ESP_OK ? true : false);
    return 1;
}

/*
twai总线恢复
@api twai.busRecovery()
@return bool
@usage
twai.busRecovery()
*/
static int l_twai_bus_recovery(lua_State *L)
{
    esp_err_t err = -1;
    err = twai_initiate_recovery();
    lua_pushboolean(L, err = ESP_OK ? true : false);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_twai[] =
    {
        {"setup", l_twai_setup, 0},
        {"close", l_twai_close, 0},
        {"send", l_twai_send, 0},
        {"recv", l_twai_recv, 0},
        {"getAlerts", l_twai_get_alerts, 0},
        {"reconfigureAlerts", l_twai_reconfigure_alerts, 0},
        {"busRecovery", l_twai_bus_recovery, 0},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_twai(lua_State *L)
{
    luat_newlib(L, reg_twai);
    return 1;
}
