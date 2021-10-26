/*
@module  wlan
@summary esp32_wifi操作库
@version 1.0
@date    2021.05.30
*/
#include "luat_base.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include <string.h>
#include "esp_event.h"
#include "luat_msgbus.h"

esp_event_handler_instance_t instance_any_id;
esp_event_handler_instance_t instance_got_ip;
//回调事件处理
static int l_wlan_handler(lua_State *L, void *ptr)
{
    rtos_msg_t *msg = (rtos_msg_t *)lua_topointer(L, -1);
    int event = msg->arg1;

    lua_getglobal(L, "sys_pub");
    if (lua_isnil(L, -1))
    {
        lua_pushinteger(L, 0);
        return 1;
    }
    ESP_LOGI("wlan","event：%d",event);
    switch (event)
    {
    case WIFI_EVENT_WIFI_READY: // 网络就绪
        lua_pushstring(L, "WLAN_READY");
        lua_call(L, 1, 0);
        // 额外发送一个通用事件 NET_READY
        lua_getglobal(L, "sys_pub");
        lua_pushstring(L, "NET_READY");
        lua_call(L, 1, 0);
        break;
    case WIFI_EVENT_STA_START: // 连上wifi路由器/热点,但还没拿到ip
        esp_wifi_connect();
        lua_pushstring(L, "WLAN_STA_CONNECTED");
        lua_pushinteger(L, 1);
        lua_call(L, 2, 0);
        break;
    case WIFI_EVENT_STA_DISCONNECTED: // 从wifi路由器/热点断开了
        lua_pushstring(L, "WLAN_STA_DISCONNECTED");
        lua_call(L, 1, 0);
        break;
    default:
        break;
    }
    lua_pushinteger(L, 0);
    return 1;
}

// 注册回调
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    rtos_msg_t msg;
    msg.handler = l_wlan_handler;
    msg.ptr = NULL;
    msg.arg1 = event_id;
    msg.arg2 = 0;
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
设置wifi模式,通常不需要设置
@api wlan.setMode(mode)
@int 模式wlan.NONE, wlan.STATION, wlan.AP,wlan.STATIONAP
@return int   设置成功与否,通常不检查
@usage 
-- 将wlan设置为wifi客户端模式
wlan.setMode(wlan.STATION) 
*/
static int l_wlan_set_mode(lua_State *L)
{
    int mode = luaL_checkinteger(L, 1);
    esp_err_t err;
    switch (mode)
    {
    case WIFI_MODE_NULL:
    case WIFI_MODE_STA:
    case WIFI_MODE_AP:
    case WIFI_MODE_APSTA:
        return ((err = esp_wifi_set_mode(mode)) == ESP_OK) ? 0 : luaL_error(L, "failed to set mode, code %d", err);
    default:
        return luaL_error(L, "invalid wifi mode %d", mode);
    }
}

/*
初始化wifi
@api wlan.init()
@return int   是否初始化成功，失败会有原因返回
@usage 
-- 在使用wifi前初始化一下
wlan.init()
*/
static int l_wlan_init(lua_State *L)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t err = esp_wifi_init(&cfg);
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));
    return (err == ESP_OK) ? 0 : luaL_error(L, "failed to init wifi, code %d", err);
}

/*
连接wifi,成功启动联网线程不等于联网成功!!
@api wlan.connect(ssid,password)
@string  ssid  wifi的SSID
@string password wifi的密码,可选
@return boolean   如果正常启动联网线程,无返回值,否则返回出错信息. 
@usage 
-- 连接到uiot,密码1234567890
wlan.connect("uiot", "1234567890")
*/
static int l_wlan_connect(lua_State *L)
{
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
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
    return (err == ESP_OK) ? 0 : luaL_error(L, "failed to begin connect, code %d", err);
}

/*
断开wifi
@api wlan.disconnect()
@return boolean 成功返回true,否则返回false
@usage
-- 断开wifi连接 
wlan.disconnect()
*/
static int l_wlan_disconnect(lua_State *L)
{
    esp_err_t err = esp_wifi_disconnect();
    return (err == ESP_OK) ? 0 : luaL_error(L, "disconnect failed, code %d", err);
}


/*
去初始化wifi
@api wlan.deinit()
@return boolean 成功返回true,否则返回false
@usage
-- 去初始化wifi
wlan.deinit()
*/
static int l_wlan_deinit(lua_State *L)
{
    esp_err_t err = esp_wifi_deinit();
    ESP_ERROR_CHECK(err);
    return (err == ESP_OK) ? 0 : luaL_error(L, "deinit failed, code %d", err);
}


#include "rotable.h"
static const rotable_Reg reg_wlan[] =
    {
        {"init", l_wlan_init, 0},
        {"getMode", l_wlan_get_mode, 0},
        {"setMode", l_wlan_set_mode, 0},
        {"connect", l_wlan_connect, 0},
        {"disconnect", l_wlan_disconnect, 0},
        {"deinit",l_wlan_deinit,0},

        {"NONE", NULL, WIFI_MODE_NULL},
        {"STATION", NULL, WIFI_MODE_STA},
        {"AP", NULL, WIFI_MODE_AP},
        {"STATIONAP", NULL, WIFI_MODE_APSTA},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_wlan(lua_State *L)
{
    luat_newlib(L, reg_wlan);
    return 1;
}
