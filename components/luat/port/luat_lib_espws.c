/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  espws
@summary ws客户端
@version 1.0
@date    2022.03.27
*/

#include "luat_base.h"
#include "luat_msgbus.h"

#include "esp_websocket_client.h"
#include "esp_event.h"

#define LUAT_LOG_TAG "espws"
#include "luat_log.h"

static int l_ws_handle(lua_State *L, void *ptr)
{
    rtos_msg_t *msg = (rtos_msg_t *)lua_topointer(L, -1);
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)ptr;
    if (data == NULL)
    {
        return 1;
    }

    lua_getglobal(L, "sys_pub");
    switch (msg->arg1)
    {
    case WEBSOCKET_EVENT_CONNECTED:
        lua_pushstring(L, "WS_CONNECT");
        lua_call(L, 1, 0);
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        lua_pushstring(L, "WS_DISCONNECT");
        lua_call(L, 1, 0);
        break;
    case WEBSOCKET_EVENT_DATA:
        lua_pushstring(L, "WS_EVENT_DATA");
        lua_pushlightuserdata(L, data->client);
        if (data->data_ptr != NULL && data->data_len >= 0)
        {
            lua_pushlstring(L, data->data_ptr, data->data_len);
            lua_pushinteger(L, data->data_len);
        }
        else
        {
            lua_pushnil(L);
            lua_pushnil(L);
        }

        lua_call(L, 4, 0);
        break;
    default:
        break;
    }
    free(data);
    return 0;
}

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    if (event_data != NULL)
    {
        rtos_msg_t msg = {0};
        esp_websocket_event_data_t *data = malloc(sizeof(esp_websocket_event_data_t));
        memcpy(data, event_data, sizeof(esp_websocket_event_data_t));
        msg.handler = l_ws_handle;
        msg.arg1 = event_id;
        msg.ptr = (void *)data;
        msg.arg2 = 0;
        luat_msgbus_put(&msg, 0);
    }
    else
    {
        LLOGE("ws event_data is null!!!");
    }
}

/*
初始化一个websocket client
@api espws.init(uri,username,password)
@string uri
@string 用户名
@string 密码
@return userdata 如果成功就返回client指针,否则返回nil
@usage
local ws = espws.init("ws://airtest.openluat.com:2900/websocket")
*/
static int l_ws_init(lua_State *L)
{
    if (!lua_isstring(L, 1))
    {
        LLOGE("please init with uri");
    }
    esp_websocket_client_config_t ws_cfg = {0};
    const char *uri = luaL_checkstring(L, 1);
    ws_cfg.uri = uri;
    if (lua_isstring(L, 2))
    {
        const char *username = luaL_checkstring(L, 2);
        ws_cfg.username = username;
    }
    if (lua_isstring(L, 3))
    {
        const char *password = luaL_checkstring(L, 3);
        ws_cfg.password = password;
    }
    esp_websocket_client_handle_t client = esp_websocket_client_init(&ws_cfg);
    if (client != NULL)
    {
        esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, client);
        lua_pushlightuserdata(L, client);
        return 1;
    }
    return 0;
}

/*
启动websocket
@api espws.start(handle)
@userdata espws.init返回的client指针
@return boolean 成功与否
@usage
espws.start(ws)
*/
static int l_ws_start(lua_State *L)
{
    if (!lua_islightuserdata(L, 1))
    {
        LLOGE("check your client , which is init by espws.init");
        return 0;
    }
    esp_websocket_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL)
    {
        LLOGE("check your client , which is init by espws.init");
        return 0;
    }
    esp_err_t err = esp_websocket_client_start(client);
    lua_pushboolean(L, err == 0 ? 1 : 0);
    return 1;
}

/*
关闭websocket
@api espws.close(handle)
@userdata espws.init返回的client指针
@return boolean 成功与否
@usage
espws.close(ws)
*/
static int l_ws_close(lua_State *L)
{
    if (!lua_islightuserdata(L, 1))
    {
        LLOGE("check your client , which is init by espws.init");
        return 0;
    }
    esp_websocket_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL)
    {
        LLOGE("check your client , which is init by espws.init");
        return 0;
    }
    esp_err_t err = esp_websocket_client_close(client, portMAX_DELAY);
    lua_pushboolean(L, err == 0 ? 1 : 0);
    return 1;
}

/*
销毁websocket
@api espws.destory(handle)
@userdata espws.init返回的client指针
@return boolean 成功与否
@usage
espws.destory(ws)
*/
static int l_ws_destory(lua_State *L)
{
    if (!lua_islightuserdata(L, 1))
    {
        LLOGE("check your client , which is init by espws.init");
        return 0;
    }
    esp_websocket_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL)
    {
        LLOGE("check your client , which is init by espws.init");
        return 0;
    }
    esp_err_t err = esp_websocket_client_destroy(client);
    lua_pushboolean(L, err == 0 ? 1 : 0);
    return 1;
}

/*
websocket发送消息
@api espws.send(handle,data)
@userdata espws.init返回的client指针
@string 要发送的消息
@return boolean 成功与否
@usage
espws.send(ws, "hello luatos esp32")
*/
static int l_ws_send(lua_State *L)
{
    if (!lua_islightuserdata(L, 1))
    {
        LLOGE("check your client , which is init by espws.init");
        return 0;
    }
    esp_websocket_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL)
    {
        LLOGE("check your client , which is init by espws.init");
        return 0;
    }
    size_t len = 0;
    const char *data = luaL_checklstring(L, 2, &len);
    int slen = esp_websocket_client_send_text(client, data, (int)len, portMAX_DELAY);
    lua_pushboolean(L, slen == len ? 1 : 0);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_espws[] =
    {
        {"init", l_ws_init, 0},
        {"start", l_ws_start, 0},
        {"close", l_ws_close, 0},
        {"destory", l_ws_destory, 0},
        {"send", l_ws_send, 0},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_espws(lua_State *L)
{
    luat_newlib(L, reg_espws);
    return 1;
}
