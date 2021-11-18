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
    espnow_event_info_t *evt = (espnow_event_info_t *)msg->ptr;
    espnow_event_send_cb_t *send_cb = evt->send_cb;
    espnow_event_recv_cb_t *recv_cb = evt->recv_cb;
    int type = msg->arg1;
    switch (type)
    {
    case 1:
        // recv
        lua_getglobal(L, "sys_pub");
        lua_pushstring(L, "ESPNOW_RECV");
        lua_pushlstring(L, (const char *)recv_cb->mac_addr, ESP_NOW_ETH_ALEN);
        lua_pushlstring(L, (const char *)recv_cb->data, recv_cb->data_len);
        lua_pushinteger(L, recv_cb->data_len);
        lua_call(L, 3, 0);
        break;
    case 2:
        // send
        lua_getglobal(L, "sys_pub");
        lua_pushstring(L, "ESPNOW_SEND");
        lua_pushlstring(L, (const char *)send_cb->mac_addr, ESP_NOW_ETH_ALEN);
        lua_pushinteger(L, send_cb->status);
        lua_call(L, 2, 0);
    default:
        break;
    }
    free(evt);
    return 0;
}

static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    rtos_msg_t msg;
    espnow_event_info_t *evt = malloc(sizeof(espnow_event_info_t));
    espnow_event_send_cb_t *send_cb = evt->send_cb;

    if (mac_addr == NULL)
    {
        ESP_LOGE(TAG, "Send cb arg error");
        return;
    }
    else
    {
        ESP_LOGD(TAG, "send ok : " MACSTR " :status:%d", MAC2STR(mac_addr), status);
        memcpy(send_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
        send_cb->status = status;

        msg.handler = l_espnow_handler;
        msg.ptr = (espnow_event_info_t *)&evt;
        msg.arg1 = 2; // send = 2
        msg.arg2 = 0;
        luat_msgbus_put(&msg, 1);
    }
}

static void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len)
{
    rtos_msg_t msg;
    espnow_event_info_t *evt = malloc(sizeof(espnow_event_info_t));
    espnow_event_recv_cb_t *recv_cb = evt->recv_cb;

    if (mac_addr == NULL || data == NULL || len <= 0)
    {
        ESP_LOGE(TAG, "Receive cb arg error");
        return;
    }
    else
    {
        memcpy(recv_cb->data, data, len);
        recv_cb->data_len = len;

        msg.handler = l_espnow_handler;
        msg.ptr = (espnow_event_info_t *)&evt;
        msg.arg1 = 1; // recv = 1
        msg.arg2 = 0;
        luat_msgbus_put(&msg, 1);
    }
}

/*
初始化espnow
@api espnow.init()
@return int  
@usage 
espnow.init()
*/
static int l_espnow_init(lua_State *L)
{
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));
    lua_pushinteger(L, 1);
    return 1;
}

/*
增加espnow peer
@api espnow.addPeer()
@string mac地址
@return int  
@usage 
espnow.addPeer(string.fromHex("0016EAAE3C40"))
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
    if (memcmp(broadcast_mac, peer_mac, 6) != 0)
    {
        // 非广播流程
        size_t len2 = 0;
        const char *lmk_key = luaL_checklstring(L, 2, &len2);
        memcpy(peer->peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
        peer->channel = 1;
        peer->ifidx = ESP_IF_WIFI_STA;
        peer->encrypt = true;
        memcpy(peer->lmk, lmk_key, ESP_NOW_KEY_LEN);
    }
    else
    {
        // 广播流程
        memcpy(peer->peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
        peer->channel = 1;
        peer->ifidx = ESP_IF_WIFI_STA;
        peer->encrypt = false;
    }
    ESP_ERROR_CHECK(esp_now_add_peer(peer));
    free(peer);
    lua_pushinteger(L, 1);
    return 1;
}

/*
espnow发送
@api espnow.send()
@string mac地址
@string 发送的数据
@return int  
@usage 
espnow.send(string.fromHex("0016EAAE3C40"),"espnow")
*/
static int l_espnow_send(lua_State *L)
{
    size_t len;
    const char *my_broadcast_mac = luaL_checklstring(L, 1, &len);
    const char *send_data = luaL_checklstring(L, 2, &len);
    if (esp_now_send((const uint8_t *)my_broadcast_mac, (uint8_t *)send_data, strlen((char *)send_data)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Send error");
        return 0;
    }
    ESP_LOGD(TAG, "send ok");
    lua_pushinteger(L, 1);
    return 1;
}

/*
去初始化espnow
@api espnow.deinit()
@return int  
@usage 
espnow.deinit()
*/
static int l_espnow_deinit(lua_State *L)
{
    esp_err_t err = esp_now_deinit();
    ESP_ERROR_CHECK(err);
    lua_pushinteger(L, 1);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_espnow[] =
    {
        {"init", l_espnow_init, 0},
        {"addPeer", l_espnow_add_peer, 0},
        {"send", l_espnow_send, 0},
        {"deinit", l_espnow_deinit, 0},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_espnow(lua_State *L)
{
    luat_newlib(L, reg_espnow);
    return 1;
}