/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  espnow
@summary espnow库
@version 1.0
@date    2022.2.15
*/
#include "luat_base.h"
#include "luat_msgbus.h"
#include "luat_log.h"
#define LUAT_LOG_TAG "luat.espnow"

#include <stdlib.h>
#include <string.h>
// #include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "espnow_handle.h"

static const char *TAG = "LESPNOW";
static const char *broadcast_mac = "\377\377\377\377\377\377"; // FF:FF:FF:FF:FF:FF

static int l_espnow_handler(lua_State *L, void *ptr)
{
    rtos_msg_t *msg = (rtos_msg_t *)lua_topointer(L, -1);
    espnow_event_handle_t *evt = (espnow_event_handle_t *)ptr;
    // ESP_LOGD(TAG, "enter handler");
    int type = msg->arg1;
    switch (type)
    {
    case 1:
        // recv
        // ESP_LOGD(TAG, "ESPNOW_RECV");
        lua_getglobal(L, "sys_pub");
        lua_pushstring(L, "ESPNOW_RECV");
        lua_pushlstring(L, (const char *)evt->recv_mac_addr, ESP_NOW_ETH_ALEN);
        lua_pushlstring(L, (const char *)evt->data, evt->data_len);
        lua_pushinteger(L, evt->data_len);
        lua_call(L, 4, 0);
        break;
    case 2:
        // send
        // ESP_LOGD(TAG, "ESPNOW_SEND");
        lua_getglobal(L, "sys_pub");
        lua_pushstring(L, "ESPNOW_SEND");
        lua_pushlstring(L, (const char *)evt->send_mac_addr, ESP_NOW_ETH_ALEN);
        lua_pushinteger(L, evt->status);
        lua_call(L, 3, 0);
        break;
    default:
        break;
    }
    free(evt);
    return 0;
}

static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    rtos_msg_t msg;
    espnow_event_handle_t *evt = malloc(sizeof(espnow_event_handle_t));

    ESP_LOGD(TAG, "send ok : " MACSTR " :status:%d", MAC2STR(mac_addr), status);
    evt->send_mac_addr = mac_addr;
    evt->status = status;

    msg.handler = l_espnow_handler;
    msg.ptr = (espnow_event_handle_t *)evt;
    msg.arg1 = 2; // send = 2
    msg.arg2 = 0;
    luat_msgbus_put(&msg, 1);
}

static void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len)
{
    rtos_msg_t msg;
    espnow_event_handle_t *evt = malloc(sizeof(espnow_event_handle_t));

    ESP_LOGD(TAG, "Recv data to " MACSTR ", len: %d ,data: %s", MAC2STR(mac_addr), len, data);
    evt->recv_mac_addr = mac_addr;
    evt->data = data;
    evt->data_len = len;

    msg.handler = l_espnow_handler;
    msg.ptr = (espnow_event_handle_t *)evt;
    msg.arg1 = 1; // recv = 1
    msg.arg2 = 0;
    luat_msgbus_put(&msg, 1);
}

/*
初始化espnow
@api espnow.init()
@return int 0成功
@usage
espnow.init()
*/
static int l_espnow_init(lua_State *L)
{
    esp_err_t err = -1;
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    err = esp_now_init();
    if (err == ESP_OK)
    {
        esp_now_register_send_cb(espnow_send_cb);
        esp_now_register_recv_cb(espnow_recv_cb);
    }

    lua_pushinteger(L, err);
    return 1;
}

/*
设置pmk
@api espnow.setPmk(pmk)
@string pmk primary master key
@return int esp_err
@usage
espnow.setPmk("pmk1234567890123")
*/
static int l_espnow_set_pmk(lua_State *L)
{
    size_t len = 0;
    const char *pmk = luaL_checklstring(L, 1, &len);
    esp_err_t err = esp_now_set_pmk((const uint8_t *)pmk);
    lua_pushinteger(L, err);
    return 1;
}

/*
增加espnow peer
@api espnow.addPeer(mac,lmk)
@string mac地址
@string lmk local master key
@return int esp_err
@usage
espnow.addPeer(string.fromHex("0016EAAE3C40"),"lmk1234567890123")
*/
static int l_espnow_add_peer(lua_State *L)
{
    size_t len = 0;
    esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
    if (peer == NULL)
    {
        ESP_LOGE(TAG, "Malloc peer information fail");
        return 0;
    }
    memset(peer, 0, sizeof(esp_now_peer_info_t));
    const char *peer_mac = luaL_checklstring(L, 1, &len);
    // printf(MACSTR"\n",MAC2STR(peer_mac));
    if (memcmp(broadcast_mac, peer_mac, 6) != 0)
    {
        // 非广播流程
        size_t len2 = 0;
        const char *lmk_key = luaL_checklstring(L, 2, &len2);
        memcpy(peer->peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
        peer->channel = 0;
        peer->ifidx = ESP_IF_WIFI_STA;
        peer->encrypt = true;
        memcpy(peer->lmk, lmk_key, ESP_NOW_KEY_LEN);
    }
    else
    {
        // 广播流程
        memcpy(peer->peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
        peer->channel = 0;
        peer->ifidx = ESP_IF_WIFI_STA;
        peer->encrypt = false;
    }
    esp_err_t err = esp_now_add_peer(peer);
    free(peer);
    lua_pushinteger(L, err);
    return 1;
}

/*
espnow发送
@api espnow.send(mac,data)
@string mac地址
@string 发送的数据
@return int  esp_err
@usage
espnow.send(string.fromHex("0016EAAE3C40"),"espnow")
*/
static int l_espnow_send(lua_State *L)
{
    size_t len = 0;
    const char *my_broadcast_mac = luaL_checklstring(L, 1, &len);
    const char *send_data = luaL_checklstring(L, 2, &len);

    esp_err_t err = esp_now_send((const uint8_t *)my_broadcast_mac, (const uint8_t *)send_data, len);
    lua_pushinteger(L, err);
    return 1;
}

/*
去初始化espnow
@api espnow.deinit()
@return int  esp_err
@usage
espnow.deinit()
*/
static int l_espnow_deinit(lua_State *L)
{
    esp_err_t err = -1;
    esp_wifi_stop();                 // todo error check
    esp_event_loop_delete_default(); // todo error check
    err = esp_now_deinit();
    lua_pushinteger(L, err);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_espnow[] =
    {
        {"init", l_espnow_init, 0},
        {"setPmk", l_espnow_set_pmk, 0},
        {"addPeer", l_espnow_add_peer, 0},
        {"send", l_espnow_send, 0},
        {"deinit", l_espnow_deinit, 0},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_espnow(lua_State *L)
{
    luat_newlib(L, reg_espnow);
    return 1;
}
