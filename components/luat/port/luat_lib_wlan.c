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
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_err_t err = esp_wifi_init(&cfg);
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
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_err_t err = esp_wifi_connect();
    return (err == ESP_OK) ? 0 : luaL_error(L, "failed to begin connect, code %d", err);
}

#include "rotable.h"
static const rotable_Reg reg_wlan[] =
    {
        {"init", l_wlan_init, 0},
        {"getMode", l_wlan_get_mode, 0},
        {"setMode", l_wlan_set_mode, 0},
        {"connect", l_wlan_connect, 0},
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
