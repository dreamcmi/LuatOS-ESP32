/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  wlan
@summary esp32_wifi操作库
@version 1.0
@date    2021.05.30
*/
#include <string.h>
#include "luat_base.h"
#include "esp_log.h"
#include "luat_timer.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "luat_msgbus.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_smartconfig.h"

static const char *TAG = "LWLAN";

static EventGroupHandle_t s_wifi_event_group;
static esp_event_handler_instance_t instance_wifi;
static esp_event_handler_instance_t instance_got_ip;
static esp_event_handler_instance_t instance_scan;

static esp_netif_t *wifi_sta_netif = NULL;
static esp_netif_t *wifi_ap_netif = NULL;
// static esp_netif_t *wifi_apsta_netif = NULL;

static bool smart_config_active = false;

static uint64_t smart_config_wait_id = 0;

typedef union
{
    ip_event_got_ip_t got_ip_event;
    smartconfig_event_got_ssid_pswd_t got_ssid_pswd_event;
} WLAN_MSG_CONTEXT;

struct
{
    uint8_t auto_connect : 1;
    uint8_t mode : 2;
    uint8_t reserved : 1;
} wlan_ini;

//回调事件处理
static int l_wlan_handler(lua_State *L, void *ptr)
{
    rtos_msg_t *msg = (rtos_msg_t *)lua_topointer(L, -1);
    int event = msg->arg1;
    int type = msg->arg2;
    wifi_event_ap_staconnected_t *evt = (wifi_event_ap_staconnected_t *)ptr;

    if (type == 0)
    {
        switch (event)
        {
        case WIFI_EVENT_STA_START:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
wlan_sta开始
WLAN_STA_START
@usage
sys.subscribe("WLAN_STA_START", function ()
    log.info("wlan", "WLAN_STA_START")
end)
*/
            lua_pushstring(L, "WLAN_STA_START");
            lua_call(L, 1, 0);
            break;
        case WIFI_EVENT_STA_CONNECTED:
            if (smart_config_wait_id != 0)
            {
                smart_config_wait_id = 0;
                lua_pushboolean(L, 1);
                luat_cbcwait(L, smart_config_wait_id, 1);
            }
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
wlan已连上wifi
WLAN_STA_CONNECTED
@usage
sys.subscribe("WLAN_STA_CONNECTED", function ()
    log.info("wlan", "WLAN_STA_CONNECTED")
end)
*/
            lua_pushstring(L, "WLAN_STA_CONNECTED");
            lua_call(L, 1, 0);
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
wlan已断开wifi
WLAN_STA_DISCONNECTED
@usage
sys.subscribe("WLAN_STA_DISCONNECTED", function ()
    log.info("wlan", "WLAN_STA_DISCONNECTED")
end)
*/
            lua_pushstring(L, "WLAN_STA_DISCONNECTED");
            if (wlan_ini.auto_connect == 1 || smart_config_active)
            {
                ESP_LOGI(TAG, "RECONNECT WIFI ");
                esp_wifi_connect();
            }
            if (smart_config_active)
            {
                xEventGroupClearBits(s_wifi_event_group, BIT0);
            }
            lua_call(L, 1, 0);
            break;
        case WIFI_EVENT_STA_STOP:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
wlan已停止
WIFI_EVENT_STA_STOP
@usage
sys.subscribe("WIFI_EVENT_STA_STOP", function ()
    log.info("wlan", "WIFI_EVENT_STA_STOP")
end)
*/
            lua_pushstring(L, "WLAN_STA_STOP");
            lua_call(L, 1, 0);
            break;
        case WIFI_EVENT_AP_START:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
wlan ap启动
WLAN_AP_START
@usage
sys.subscribe("WLAN_AP_START", function ()
    log.info("wlan", "WLAN_AP_START")
end)
*/
            lua_pushstring(L, "WLAN_AP_START");
            lua_call(L, 1, 0);
            break;
        case WIFI_EVENT_AP_STOP:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
wlan ap停止
WLAN_AP_STOP
@usage
sys.subscribe("WLAN_AP_STOP", function ()
    log.info("wlan", "WLAN_AP_STOP")
end)
*/
            lua_pushstring(L, "WLAN_AP_STOP");
            lua_call(L, 1, 0);
            break;
        case WIFI_EVENT_AP_STACONNECTED:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
wlan ap有客户端连接
WLAN_AP_STACONNECTED
@string mac地址
@int aid
@usage
sys.subscribe("WLAN_AP_STACONNECTED", function (mac,aid)
    log.info("wlan", "WLAN_AP_STACONNECTED",aid,mac:toHex())
end)
*/
            lua_pushstring(L, "WLAN_AP_STACONNECTED");
            lua_pushlstring(L, (const char *)evt->mac, 6);
            lua_pushinteger(L, evt->aid);
            lua_call(L, 3, 0);
            free(evt);
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
wlan ap有客户端断开
WLAN_AP_STADISCONNECTED
@string mac地址
@int aid
@usage
sys.subscribe("WLAN_AP_STADISCONNECTED", function (mac,aid)
    log.info("wlan", "WLAN_AP_STADISCONNECTED",aid,mac:toHex())
end)
*/
            lua_pushstring(L, "WLAN_AP_STADISCONNECTED");
            lua_pushlstring(L, (const char *)evt->mac, 6);
            lua_pushinteger(L, evt->aid);
            lua_call(L, 3, 0);
            free(evt);
            break;
        default:
            break;
        }
    }
    else if (type == 1)
    {
        ip_event_got_ip_t *event_data = (ip_event_got_ip_t *)ptr;
        switch (event)
        {
        case IP_EVENT_STA_GOT_IP:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
获取到ip
IP_READY
@string ip地址
@usage
sys.subscribe("IP_READY", function (ip)
    log.info("wlan", "IP_READY",ip)
end)
*/
            lua_pushstring(L, "IP_READY");
            lua_pushfstring(L, "%d.%d.%d.%d", esp_ip4_addr1_16(&event_data->ip_info.ip),
                            esp_ip4_addr2_16(&event_data->ip_info.ip),
                            esp_ip4_addr3_16(&event_data->ip_info.ip),
                            esp_ip4_addr4_16(&event_data->ip_info.ip));
            free(ptr); //这里的ptr是深拷贝，需要释放
            lua_call(L, 2, 0);
            break;
        default:
            break;
        }
    }
    else if (type == 2)
    {
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)ptr;
        wifi_config_t wifi_config = {0};
        switch (event)
        {
        case SC_EVENT_SCAN_DONE:
            lua_getglobal(L, "sys_pub");
            /*
            @sys_pub wlan
            smartconfig扫描完成
            WIFI_SCAN_DONE
            @usage
            sys.subscribe("WIFI_SCAN_DONE", function ()
                log.info("wlan", "WIFI_SCAN_DONE")
            end)
            */
            lua_pushstring(L, "WIFI_SCAN_DONE");
            lua_call(L, 1, 0);
            break;
        case SC_EVENT_FOUND_CHANNEL:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
smartconfig获取到目标ap的channle
WIFI_SCAN_FOUND_CHANNEL
@usage
sys.subscribe("WIFI_SCAN_FOUND_CHANNEL", function ()
    log.info("wlan", "WIFI_SCAN_FOUND_CHANNEL")
end)
*/
            lua_pushstring(L, "WIFI_SCAN_FOUND_CHANNEL");
            lua_call(L, 1, 0);
            break;
        case SC_EVENT_GOT_SSID_PSWD:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
smartconfig获取到wifi信息
WIFI_SCAN_GOT_SSID_PSWD
@usage
sys.subscribe("WIFI_SCAN_GOT_SSID_PSWD", function ()
    log.info("wlan", "WIFI_SCAN_GOT_SSID_PSWD")
end)
*/
            lua_pushstring(L, "WIFI_SCAN_GOT_SSID_PSWD");
            lua_call(L, 1, 0);
            memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
            memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
            wifi_config.sta.bssid_set = evt->bssid_set;
            if (wifi_config.sta.bssid_set == true)
            {
                memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
            }
            ESP_LOGI(TAG, "SC_EVENT_GOT_SSID_PSWD");
            ESP_LOGI(TAG, "got SSID:%s", wifi_config.sta.ssid);
            ESP_LOGI(TAG, "got PASSWORD:%s", wifi_config.sta.password);
            esp_wifi_disconnect();                          // todo error check
            esp_wifi_set_config(WIFI_IF_STA, &wifi_config); // todo error check
            esp_wifi_connect();
            free(ptr); //这里的ptr是深拷贝，需要释放
            break;
        case SC_EVENT_SEND_ACK_DONE:
            lua_getglobal(L, "sys_pub");
/*
@sys_pub wlan
smartconfig ack发送完成
SMARTCONFIG_ACK_DONE
@usage
sys.subscribe("SMARTCONFIG_ACK_DONE", function ()
    log.info("wlan", "SMARTCONFIG_ACK_DONE")
end)
*/
            lua_pushstring(L, "SMARTCONFIG_ACK_DONE");
            lua_call(L, 1, 0);
            break;
        default:
            break;
        }
    }

    lua_pushinteger(L, 0);
    return 1;
}

// 注册回调
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    rtos_msg_t msg = {0};
    msg.handler = l_wlan_handler;
    msg.ptr = NULL;
    ESP_LOGI(TAG, "event_handler %s %d", event_base, event_id);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        msg.arg1 = WIFI_EVENT_STA_START;
        msg.arg2 = 0;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        msg.arg1 = WIFI_EVENT_STA_DISCONNECTED;
        msg.arg2 = 0;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        msg.arg1 = WIFI_EVENT_STA_CONNECTED;
        msg.arg2 = 0;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP)
    {
        msg.arg1 = WIFI_EVENT_STA_STOP;
        msg.arg2 = 0;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_START)
    {
        msg.arg1 = WIFI_EVENT_AP_START;
        msg.arg2 = 0;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STOP)
    {
        msg.arg1 = WIFI_EVENT_AP_STOP;
        msg.arg2 = 0;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *evt = malloc(sizeof(wifi_event_ap_staconnected_t));
        memcpy(evt, event_data, sizeof(wifi_event_ap_staconnected_t));
        msg.arg1 = WIFI_EVENT_AP_STACONNECTED;
        msg.arg2 = 0;
        msg.ptr = evt;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_staconnected_t *evt = malloc(sizeof(wifi_event_ap_staconnected_t));
        memcpy(evt, event_data, sizeof(wifi_event_ap_staconnected_t));
        msg.arg1 = WIFI_EVENT_AP_STADISCONNECTED;
        msg.arg2 = 0;
        msg.ptr = evt;
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        WLAN_MSG_CONTEXT *msg_context = (WLAN_MSG_CONTEXT *)malloc(sizeof(WLAN_MSG_CONTEXT));
        //这里需要深拷贝
        memcpy(msg_context, event_data, sizeof(smartconfig_event_got_ssid_pswd_t));
        msg.arg1 = IP_EVENT_STA_GOT_IP;
        msg.arg2 = 1;
        msg.ptr = msg_context;
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
    {
        msg.arg1 = SC_EVENT_SCAN_DONE;
        msg.arg2 = 2;
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
    {
        msg.arg1 = SC_EVENT_FOUND_CHANNEL;
        msg.arg2 = 2;
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
    {
        WLAN_MSG_CONTEXT *msg_context = (WLAN_MSG_CONTEXT *)malloc(sizeof(WLAN_MSG_CONTEXT));
        //这里需要深拷贝
        memcpy(msg_context, event_data, sizeof(smartconfig_event_got_ssid_pswd_t));
        msg.arg1 = SC_EVENT_GOT_SSID_PSWD;
        msg.arg2 = 2;
        msg.ptr = msg_context;
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
    {
        msg.arg1 = SC_EVENT_SEND_ACK_DONE;
        msg.arg2 = 2;
        xEventGroupSetBits(s_wifi_event_group, BIT1);
    }
    luat_msgbus_put(&msg, 1);
}

/*
获取wifi模式
@api wlan.getMode()
@return int 模式wlan.NONE, wlan.STATION, wlan.AP,wlan.STATIONAP
@usage
-- 获取wifi的当前模式
local m = wlan.getMode()
*/
static int l_wlan_get_mode(lua_State *L)
{
    wifi_mode_t mode;
    esp_err_t err = esp_wifi_get_mode(&mode);
    if (err != ESP_OK)
        return luaL_error(L, "failed to get mode, code %d", err);
    lua_pushinteger(L, mode);
    return 1;
}

/*
设置wifi模式
@api wlan.setMode(mode)
@int 模式wlan.NONE, wlan.STATION, wlan.AP,wlan.STATIONAP
@return int   返回esp_err
@usage
-- 将wlan设置为wifi客户端模式
wlan.setMode(wlan.STATION)
*/
static int l_wlan_set_mode(lua_State *L)
{
    int mode = luaL_checkinteger(L, 1);
    wlan_ini.mode = (char)mode;
    esp_err_t err = -1;
    if (mode < WIFI_MODE_MAX)
    {
        err = esp_wifi_set_mode(mode);
        switch (mode)
        {
        case WIFI_MODE_STA:
            if (!wifi_sta_netif)
            {
                wifi_sta_netif = esp_netif_create_default_wifi_sta();
            }
            break;
        case WIFI_MODE_AP:
            if (!wifi_ap_netif)
            {
                wifi_ap_netif = esp_netif_create_default_wifi_ap();
            }
            break;
        default:
            break;
        }
        lua_pushinteger(L, err);
    }
    else
    {
        lua_pushinteger(L, ESP_ERR_INVALID_ARG);
    }
    return 1;
}

/*
初始化wifi
@api wlan.init()
@return int esp_err
@usage
-- 在使用wifi前初始化一下
wlan.init()
*/
static int l_wlan_init(lua_State *L)
{
    esp_netif_init(); // todo error check
    s_wifi_event_group = xEventGroupCreate();
    esp_event_loop_create_default(); // todo error check
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_wifi);    // todo error check
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip); // todo error check
    esp_event_handler_instance_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_scan);      // todo error check
    esp_err_t err = esp_wifi_init(&cfg);
    lua_pushinteger(L, err);
    return 1;
}

/*
连接wifi,成功启动联网线程不等于联网成功!!
@api wlan.connect(ssid,password,autoreconnect)
@string ssid  wifi的SSID
@string password wifi的密码,可选
@int 断连自动重连 1:启用 0:不启用
@return int esp_err
@usage
-- 连接到uiot,密码1234567890
wlan.connect("uiot", "1234567890")
*/
static int l_wlan_connect(lua_State *L)
{
    wifi_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    size_t len = 0;

    const char *Lssid = luaL_checklstring(L, 1, &len);
    if (len > sizeof(cfg.sta.ssid))
        len = sizeof(cfg.sta.ssid);
    strncpy((char *)cfg.sta.ssid, Lssid, len);

    const char *Lpasswd = luaL_checklstring(L, 2, &len);
    if (len > sizeof(cfg.sta.password))
        len = sizeof(cfg.sta.password);
    strncpy((char *)cfg.sta.password, Lpasswd, len);

    wlan_ini.auto_connect = luaL_optinteger(L, 3, 0);
    esp_wifi_set_config(WIFI_IF_STA, &cfg); // todo error check
    esp_wifi_start();
    esp_err_t err = esp_wifi_connect();
    lua_pushinteger(L, err);
    return 1;
}

/*
创建ap
@api wlan.createAP(ssid,password)
@string ssid  wifi的SSID
@string password wifi的密码
@int channle 信道 默认11
@int 最大连接数 默认5
@int authmode 密码验证模式
@return int esp_err
@usage
wlan.createAP("LuatOS-ESP32","12345678")
*/
static int l_wlan_create_ap(lua_State *L)
{
    wifi_config_t cfg = {0};
    size_t len = 0;

    const char *Lssid = luaL_checklstring(L, 1, &len);
    if (len > sizeof(cfg.ap.ssid))
        len = sizeof(cfg.ap.ssid);
    strncpy((char *)cfg.ap.ssid, Lssid, len);
    cfg.ap.ssid_len = len;

    const char *Lpasswd = luaL_checklstring(L, 2, &len);
    if (len > sizeof(cfg.ap.password))
        len = sizeof(cfg.ap.password);
    strncpy((char *)cfg.ap.password, Lpasswd, len);

    cfg.ap.channel = luaL_optinteger(L, 3, 11);
    cfg.ap.max_connection = luaL_optinteger(L, 4, 5);
    cfg.ap.authmode = luaL_optinteger(L, 5, WIFI_AUTH_WPA_WPA2_PSK);

    esp_wifi_set_config(WIFI_IF_AP, &cfg); // todo error check
    esp_err_t err = esp_wifi_start();
    lua_pushinteger(L, err);
    return 1;
}

/*
断开wifi
@api wlan.disconnect()
@return int esp_err
@usage
-- 断开wifi连接
wlan.disconnect()
*/
static int l_wlan_disconnect(lua_State *L)
{
    wlan_ini.auto_connect = 0; // 人为断开wifi就关掉自动重连
    esp_err_t err = esp_wifi_disconnect();
    lua_pushinteger(L, err);
    return 1;
}

/*
关闭wifi
@api wlan.stop()
@return int esp_err
@usage
-- 关闭wifi
wlan.stop()
*/
static int l_wlan_stop(lua_State *L)
{
    esp_err_t err = -1;
    err = esp_wifi_stop();
    lua_pushinteger(L, err);
    return 1;
}

/*
去初始化wifi
@api wlan.deinit()
@return int esp_err
@usage
-- 去初始化wifi
wlan.deinit()
*/
static int l_wlan_deinit(lua_State *L)
{
    esp_err_t err = -1;
    esp_wifi_stop();                 // todo error check
    esp_event_loop_delete_default(); // todo error check
    if (wlan_ini.mode == WIFI_MODE_STA)
    {
        esp_netif_destroy_default_wifi(wifi_sta_netif);
    }
    else if (wlan_ini.mode == WIFI_MODE_AP)
    {
        esp_netif_destroy_default_wifi(wifi_ap_netif);
    }
    err = esp_wifi_deinit();
    lua_pushinteger(L, err);
    return 1;
}

/*
设置wifi省电
@api wlan.setps(ID)
@int 省电等级 省电等级 wlan.PS_NONE  wlan.PS_MIN_MODEM wlan.PS_MAX_MODEM
@return int esp_err
@usage
wlan.setps(wlan.PS_MAX_MODEM)
*/
static int l_wlan_set_ps(lua_State *L)
{
    esp_err_t err = esp_wifi_set_ps(luaL_checkinteger(L, 1));
    lua_pushinteger(L, err);
    return 1;
}

/*
获取wifi省电模式
@api wlan.getps()
@return int esp_err
@usage  省电等级 wlan.PS_NONE  wlan.PS_MIN_MODEM wlan.PS_MAX_MODEM
wlan.getps()
*/
static int l_wlan_get_ps(lua_State *L)
{
    wifi_ps_type_t type;
    esp_wifi_get_ps(&type);
    lua_pushinteger(L, (int)type);
    return 1;
}

/*
设置wifi协议
@api wlan.setProtocol(mode,protocaolmap)
@int mode wlan.IF_STA wlan.IF_AP
@int protocaolmap wlan.P11B wlan.P11G wlan.P11N wlan.LR
@return int esp_err
*/
static int l_wlan_set_protocol(lua_State *L)
{
    int mode = luaL_checkinteger(L, 1);
    int map = luaL_checkinteger(L, 2);
    esp_err_t err = esp_wifi_set_protocol(mode, map);
    lua_pushinteger(L, err);
    return 1;
}

/*
获取wifi协议
@api wlan.getProtocol(mode)
@int mode wlan.IF_STA wlan.IF_AP
@return int protocaolmap
*/
static int l_wlan_get_protocol(lua_State *L)
{
    int mode = luaL_checkinteger(L, 1);
    uint8_t map = 0;
    esp_wifi_get_protocol(mode, &map);
    lua_pushinteger(L, map);
    return 1;
}

/*
设置wifi带宽
@api wlan.setBandwidth(mode,bw)
@int mode wlan.IF_STA wlan.IF_AP
@int bw wlan.HT20 wlan.HT40
@return int esp_err
*/
static int l_wlan_set_bandwidth(lua_State *L)
{
    int mode = luaL_checkinteger(L, 1);
    int bw = luaL_checkinteger(L, 2);
    esp_err_t err = esp_wifi_set_bandwidth(mode, (wifi_bandwidth_t)bw);
    lua_pushinteger(L, err);
    return 1;
}

/*
获取wifi带宽
@api wlan.getBandwidth(mode)
@int mode wlan.IF_STA wlan.IF_AP
@return int bw
*/
static int l_wlan_get_bandwidth(lua_State *L)
{
    int mode = luaL_checkinteger(L, 1);
    wifi_bandwidth_t bw;
    esp_wifi_get_bandwidth(mode, &bw);
    lua_pushinteger(L, (int)bw);
    return 1;
}

static void smartconfig_task(void *parm)
{
    smartconfig_type_t mode = (smartconfig_type_t)parm;
    EventBits_t uxBits;
    esp_smartconfig_set_type(mode); // todo error check
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    esp_smartconfig_start(&cfg); // todo error check
    while (1)
    {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, BIT1, true, false, portMAX_DELAY);

        if (uxBits & BIT0)
        {
            ESP_LOGI(TAG, "WiFi Connected to ap");
        }
        if (uxBits & BIT1)
        {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}

/*
smartconfig配网(默认esptouch)
@api wlan.smartconfig(mode)
@int mode 0:ESPTouch 1:AirKiss 2:ESPTouch and AirKiss 3:ESPTouch v2
@return int 创建成功0 失败1
@usage
wlan.smartconfig()
*/
static int l_wlan_smartconfig(lua_State *L)
{
    int mode = luaL_optinteger(L, 1, SC_TYPE_ESPTOUCH);
    smart_config_active = true;
    esp_wifi_start();
    BaseType_t re = xTaskCreate(smartconfig_task, "smartconfig_task", 4096, (void *)mode, 3, NULL);
    lua_pushinteger(L, re == pdPASS ? 0 : 1);
    return 1;
}

static int smartconfig_timeout_handler(lua_State *L, void *ptr)
{
    luat_timer_t *timer = (luat_timer_t *)ptr;
    uint64_t *idp = (uint64_t *)timer->id;
    if (smart_config_wait_id != 0)
    {
        lua_pushboolean(L, 0);
        luat_cbcwait(L, *idp, 1);
        smart_config_wait_id = 0;
    }
    free(idp);
    return 0;
}

/*
smartconfig配网(默认esptouch)（多任务内使用）
@api wlan.taskSmartconfig()
@int mode，可选，默认0 0:ESPTouch 1:AirKiss 2:ESPTouch and AirKiss 3:ESPTouch v2
@int 超时时间（秒），可选，默认120秒
@return bool 联网成功true 联网失败false
@usage
--任务内调用
local result = wlan.taskSmartconfig().wait()
*/
static int l_wlan_task_smartconfig(lua_State *L)
{
    if (smart_config_wait_id != 0) //防止开启多次
    {
        lua_pushboolean(L, 0);
        luat_pushcwait_error(L, 1);
        return 1;
    }
    int mode = luaL_optinteger(L, 1, SC_TYPE_ESPTOUCH);
    int timeout = luaL_optinteger(L, 2, 120);
    smart_config_active = true;
    esp_wifi_start();
    BaseType_t re = xTaskCreate(smartconfig_task, "smartconfig_task", 4096, (void *)mode, 3, NULL);
    if (re != pdPASS) //发起失败
    {
        lua_pushboolean(L, 0);
        luat_pushcwait_error(L, 1);
        return 1;
    }
    smart_config_wait_id = luat_pushcwait(L);
    luat_timer_t *timer = (luat_timer_t *)malloc(sizeof(luat_timer_t));
    uint64_t *idp = (uint64_t *)malloc(sizeof(uint64_t));
    memcpy(idp, &smart_config_wait_id, sizeof(uint64_t));
    timer->id = (size_t)idp;
    timer->timeout = timeout * 1000;
    timer->repeat = 0;
    timer->func = &smartconfig_timeout_handler;
    luat_timer_start(timer);

    return 1;
}

// /*
// smartconfig配网停止
// @api wlan.smartconfigStop()
// @return int 创建成功0 失败1
// @usage
// wlan.smartconfigStop()
// */
// static int l_wlan_smartconfig_stop(lua_State *L)
// {
//     esp_err_t err = esp_smartconfig_stop();
//     lua_pushinteger(L, err);
//     return 1;
// }

/*
wlan dhcp开关
@api wlan.dhcp(mode)
@int 0:关闭dhcp 1:开启dhcp
@return int esp_err
@usage
wlan.dhcp(0) -- 关闭dhcp
*/
static int l_wlan_dhcp(lua_State *L)
{
    esp_err_t err = -1;
    int mode = luaL_checkinteger(L, 1);

    if (wlan_ini.mode == WIFI_MODE_STA)
    {
        switch (mode)
        {
        case 0:
            err = esp_netif_dhcpc_stop(wifi_sta_netif);
            break;
        case 1:
            err = esp_netif_dhcpc_start(wifi_sta_netif);
            break;
        default:
            break;
        }
    }
    else if (wlan_ini.mode == WIFI_MODE_AP)
    {
        switch (mode)
        {
        case 0:
            err = esp_netif_dhcps_stop(wifi_ap_netif);
            break;
        case 1:
            err = esp_netif_dhcps_start(wifi_ap_netif);
            break;
        default:
            break;
        }
    }
    lua_pushinteger(L, err);
    return 1;
}

/*
wlan设置ip信息
@api wlan.setIp(ip,gw,netmask)
@string ip ip地址 格式"xxx.xxx.xxx.xxx"
@string gw 网关地址 格式"xxx.xxx.xxx.xxx"
@string netmask 子网掩码 格式"xxx.xxx.xxx.xxx"
@return int esp_err
@usage
wlan.setIp("192.168.55.1", "192.168.55.1", "255.255.255.0")
*/
static int l_wlan_set_ip(lua_State *L)
{
    esp_err_t err = -1;
    esp_netif_ip_info_t ip_info = {0};
    int a, b, c, d;

    const char *ip = luaL_checkstring(L, 1);
    const char *gw = luaL_checkstring(L, 2);
    const char *netmask = luaL_checkstring(L, 3);

    sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d);
    IP4_ADDR(&ip_info.ip, a, b, c, d);

    sscanf(gw, "%d.%d.%d.%d", &a, &b, &c, &d);
    IP4_ADDR(&ip_info.gw, a, b, c, d);

    sscanf(netmask, "%d.%d.%d.%d", &a, &b, &c, &d);
    IP4_ADDR(&ip_info.netmask, a, b, c, d);
    if (wlan_ini.mode == WIFI_MODE_STA)
    {
        err = esp_netif_set_ip_info(wifi_sta_netif, &ip_info);
    }
    else if (wlan_ini.mode == WIFI_MODE_AP)
    {
        err = esp_netif_set_ip_info(wifi_ap_netif, &ip_info);
    }
    lua_pushinteger(L, err);
    return 1;
}

/*
wlan设置hostname
@api wlan.setHostname(name)
@string hosetname 主机名
@return int esp_err
@usage
wlan.setHostname("luatos")
*/
static int l_wlan_set_hostname(lua_State *L)
{
    esp_err_t err = -1;
    const char *name = luaL_checkstring(L, 1);
    if (wlan_ini.mode == WIFI_MODE_STA)
    {
        err = esp_netif_set_hostname(wifi_sta_netif, name);
    }
    else if (wlan_ini.mode == WIFI_MODE_AP)
    {
        err = esp_netif_set_hostname(wifi_ap_netif, name);
    }
    lua_pushinteger(L, err);
    return 1;
}

/*
wlan获取配置信息
@api  wlan.getConfig(mode)
@int mode STA:0 AP:1
@return table STA:{"ssid":"xxx","password":"xxx","bssid":"xxx"} AP:{"ssid":"xxx","password":"xxx"}
@usage
-- AP
t = wlan.getConfig(1)
log.info("wlan", "wifi ap info", t.ssid, t.password)
-- STA
t = wlan.getConfig(0)
log.info("wlan", "wifi connected info", t.ssid, t.password, t.bssid:toHex())
*/
static int l_wlan_get_config(lua_State *L)
{
    int mode = luaL_checkinteger(L, 1);
    wifi_config_t conf = {0};
    esp_wifi_get_config((wifi_interface_t)mode, &conf);

    lua_newtable(L);
    if (mode == 0)
    {
        lua_pushstring(L, "ssid");
        lua_pushstring(L, (const char *)conf.sta.ssid);
        lua_settable(L, -3);

        lua_pushstring(L, "password");
        lua_pushstring(L, (const char *)conf.sta.password);
        lua_settable(L, -3);

        lua_pushstring(L, "bssid");
        lua_pushstring(L, (const char *)conf.sta.bssid);
        lua_settable(L, -3);
    }
    else if (mode == 1)
    {
        lua_pushstring(L, "ssid");
        lua_pushstring(L, (const char *)conf.ap.ssid);
        lua_settable(L, -3);

        lua_pushstring(L, "password");
        lua_pushstring(L, (const char *)conf.ap.password);
        lua_settable(L, -3);
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_wlan[] =
    {
        {"init", l_wlan_init, 0},
        {"getMode", l_wlan_get_mode, 0},
        {"setMode", l_wlan_set_mode, 0},
        {"connect", l_wlan_connect, 0},
        {"createAP", l_wlan_create_ap, 0},
        {"disconnect", l_wlan_disconnect, 0},
        {"stop", l_wlan_stop, 0},
        {"deinit", l_wlan_deinit, 0},
        {"setps", l_wlan_set_ps, 0},
        {"getps", l_wlan_get_ps, 0},
        {"setProtocol", l_wlan_set_protocol, 0},
        {"getProtocol", l_wlan_get_protocol, 0},
        {"setBandwidth", l_wlan_set_bandwidth, 0},
        {"getBandwidth", l_wlan_get_bandwidth, 0},
        {"dhcp", l_wlan_dhcp, 0},
        {"setIp", l_wlan_set_ip, 0},
        {"getConfig", l_wlan_get_config, 0},
        {"setHostname", l_wlan_set_hostname, 0},
        {"smartconfig", l_wlan_smartconfig, 0},
        {"taskSmartconfig", l_wlan_task_smartconfig, 0},
        // {"smartconfigStop", l_wlan_smartconfig_stop, 0},

        {"NONE", NULL, WIFI_MODE_NULL},
        {"STATION", NULL, WIFI_MODE_STA},
        {"AP", NULL, WIFI_MODE_AP},
        {"STATIONAP", NULL, WIFI_MODE_APSTA},

        {"IF_STA", NULL, WIFI_IF_STA},
        {"IF_AP", NULL, WIFI_IF_AP},

        {"PS_NONE", NULL, WIFI_PS_NONE},
        {"PS_MIN_MODEM", NULL, WIFI_PS_MIN_MODEM},
        {"PS_MAX_MODEM", NULL, WIFI_PS_MAX_MODEM},

        {"AUTH_OPEN", NULL, WIFI_AUTH_OPEN},
        {"AUTH_WEP", NULL, WIFI_AUTH_WEP},
        {"AUTH_WPA_PSK", NULL, WIFI_AUTH_WPA_PSK},
        {"AUTH_WPA2_PSK", NULL, WIFI_AUTH_WPA2_PSK},
        {"AUTH_WPA_WPA2_PSK", NULL, WIFI_AUTH_WPA_WPA2_PSK},
        {"AUTH_WPA2_ENTERPRISE", NULL, WIFI_AUTH_WPA2_ENTERPRISE},
        {"AUTH_WPA3_PSK", NULL, WIFI_AUTH_WPA3_PSK},
        {"AUTH_WPA2_WPA3_PSK", NULL, WIFI_AUTH_WPA2_WPA3_PSK},
        {"AUTH_WAPI_PSK", NULL, WIFI_AUTH_WAPI_PSK},

        {"P11B", NULL, WIFI_PROTOCOL_11B},
        {"P11G", NULL, WIFI_PROTOCOL_11G},
        {"P11N", NULL, WIFI_PROTOCOL_11N},
        {"LR", NULL, WIFI_PROTOCOL_LR},

        {"HT20", NULL, WIFI_BW_HT20},
        {"HT40", NULL, WIFI_BW_HT40},

        {NULL, NULL, 0}};

LUAMOD_API int luaopen_wlan(lua_State *L)
{
    luat_newlib(L, reg_wlan);
    return 1;
}
