/*
@module  wlan
@summary esp32_wifi操作库
@version 1.0
@date    2021.05.30
*/
#include <string.h>
#include "luat_base.h"
#include "esp_log.h"
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
esp_event_handler_instance_t instance_wifi;
esp_event_handler_instance_t instance_got_ip;
esp_event_handler_instance_t instance_scan;

//回调事件处理
static int l_wlan_handler(lua_State *L, void *ptr)
{
    rtos_msg_t *msg = (rtos_msg_t *)lua_topointer(L, -1);
    int event = msg->arg1;
    int type = msg->arg2;

    if (type == 0)
    {
        switch (event)
        {
        case WIFI_EVENT_STA_START: // 网络就绪，可以链接wifi
            lua_getglobal(L, "sys_pub");
            lua_pushstring(L, "WLAN_READY");
            lua_call(L, 1, 0);
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_CONNECTED: // 已连上wifi
            lua_getglobal(L, "sys_pub");
            lua_pushstring(L, "WLAN_STA_CONNECTED");
            lua_call(L, 1, 0);
            break;
        case WIFI_EVENT_STA_DISCONNECTED: //已断开wifi
            lua_getglobal(L, "sys_pub");
            lua_pushstring(L, "WLAN_STA_DISCONNECTED");
            lua_call(L, 1, 0);
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
        case IP_EVENT_STA_GOT_IP: //已获得ip
            lua_getglobal(L, "sys_pub");
            lua_pushstring(L, "IP_READY");
            lua_pushfstring(L, "%d.%d.%d.%d", esp_ip4_addr1_16(&event_data->ip_info.ip),
                            esp_ip4_addr2_16(&event_data->ip_info.ip),
                            esp_ip4_addr3_16(&event_data->ip_info.ip),
                            esp_ip4_addr4_16(&event_data->ip_info.ip));
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
        uint8_t ssid[33] = {0};
        uint8_t password[65] = {0};
        switch (event)
        {
        case SC_EVENT_SCAN_DONE:
            lua_getglobal(L, "sys_pub");
            lua_pushstring(L, "WIFI_SCAN_DONE");
            lua_call(L, 1, 0);
            break;
        case SC_EVENT_FOUND_CHANNEL:
            lua_getglobal(L, "sys_pub");
            lua_pushstring(L, "WIFI_SCAN_FOUND_CHANNEL");
            lua_call(L, 1, 0);
            break;
        case SC_EVENT_GOT_SSID_PSWD:
            lua_getglobal(L, "sys_pub");
            lua_pushstring(L, "S");
            lua_call(L, 1, 0);
            memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
            memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
            wifi_config.sta.bssid_set = evt->bssid_set;
            if (wifi_config.sta.bssid_set == true)
            {
                memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
            }
            memcpy(ssid, evt->ssid, sizeof(evt->ssid));
            memcpy(password, evt->password, sizeof(evt->password));
            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
            esp_wifi_connect();
            break;
        case SC_EVENT_SEND_ACK_DONE:
            lua_getglobal(L, "sys_pub");
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
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        msg.arg1 = IP_EVENT_STA_GOT_IP;
        msg.arg2 = 1;
        msg.ptr = event_data;
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
        msg.arg1 = SC_EVENT_GOT_SSID_PSWD;
        msg.arg2 = 2;
        msg.ptr = event_data;
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
    esp_err_t err = -1;
    switch (mode)
    {
    case WIFI_MODE_NULL:
    case WIFI_MODE_STA:
    case WIFI_MODE_AP:
    case WIFI_MODE_APSTA:
        err = esp_wifi_set_mode(mode);
        lua_pushinteger(L, err);
        return 1;
    default:
        return luaL_error(L, "invalid wifi mode %d", mode);
    }
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
    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t err = esp_wifi_init(&cfg);
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_wifi));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_scan));
    lua_pushinteger(L, err);
    return 1;
}

/*
连接wifi,成功启动联网线程不等于联网成功!!
@api wlan.connect(ssid,password)
@string ssid  wifi的SSID
@string password wifi的密码,可选
@return int esp_err
@usage 
-- 连接到uiot,密码1234567890
wlan.connect("uiot", "1234567890")
*/
static int l_wlan_connect(lua_State *L)
{
    esp_netif_create_default_wifi_sta();
    wifi_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    size_t len;

    const char *Lssid = luaL_checklstring(L, 1, &len);
    if (len > sizeof(cfg.sta.ssid))
        len = sizeof(cfg.sta.ssid);
    strncpy((char *)cfg.sta.ssid, Lssid, len);

    const char *Lpasswd = luaL_checklstring(L, 2, &len);
    if (len > sizeof(cfg.sta.password))
        len = sizeof(cfg.sta.password);
    strncpy((char *)cfg.sta.password, Lpasswd, len);

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));
    esp_err_t err = (esp_wifi_start());
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
    esp_err_t err = esp_wifi_disconnect();
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
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
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
wlan.setps(1)
*/
static int l_wlan_set_ps(lua_State *L)
{
    int ps = luaL_checkinteger(L, 1);
    esp_err_t err = esp_wifi_set_ps(ps);
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
    esp_err_t err = -1;
    wifi_ps_type_t type;
    err = esp_wifi_get_ps(&type);
    lua_pushinteger(L, err);
    return 1;
}

static void smartconfig_task(void *parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
    while (1)
    {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, BIT0 | BIT1, true, false, portMAX_DELAY);
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
@api wlan.smartconfig()
@return int 创建成功0 失败1
@usage 
wlan.smartconfigStop()
*/
static int l_wlan_smartconfig(lua_State *L)
{
    esp_wifi_start();
    BaseType_t re = xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 12, NULL);
    lua_pushinteger(L, re == pdPASS ? 0 : 1);
    return 1;
}

/*
smartconfig配网停止
@api wlan.smartconfigStop()
@return int esp_err
@usage 
wlan.smartconfig()
*/
static int l_wlan_smartconfig_stop(lua_State *L)
{
    esp_err_t err = esp_smartconfig_stop();
    lua_pushinteger(L, err);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_wlan[] =
    {
        {"init", l_wlan_init, 0},
        {"getMode", l_wlan_get_mode, 0},
        {"setMode", l_wlan_set_mode, 0},
        {"connect", l_wlan_connect, 0},
        {"disconnect", l_wlan_disconnect, 0},
        {"deinit", l_wlan_deinit, 0},
        {"setps", l_wlan_set_ps, 0},
        {"getps", l_wlan_get_ps, 0},
        {"smartconfig", l_wlan_smartconfig, 0},
        {"smartconfigStop", l_wlan_smartconfig_stop, 0},

        {"NONE", NULL, WIFI_MODE_NULL},
        {"STATION", NULL, WIFI_MODE_STA},
        {"AP", NULL, WIFI_MODE_AP},
        {"STATIONAP", NULL, WIFI_MODE_APSTA},
        {"PS_NONE", NULL, WIFI_PS_NONE},
        {"PS_MIN_MODEM", NULL, WIFI_PS_MIN_MODEM},
        {"PS_MAX_MODEM", NULL, WIFI_PS_MAX_MODEM},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_wlan(lua_State *L)
{
    luat_newlib(L, reg_wlan);
    return 1;
}
