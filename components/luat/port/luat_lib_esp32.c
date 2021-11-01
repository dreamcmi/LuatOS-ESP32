#include "luat_base.h"
#include "esp_log.h"
#include "esp_system.h"
#include <string.h>

/*
获取mac
@api esp32.getmac()
@int mac地址来源 0:ESP_MAC_WIFI_STA 1:ESP_MAC_WIFI_SOFTAP 2:ESP_MAC_BT 3:ESP_MAC_ETH
@return string mac地址
@usage
esp32.getmac(0)
*/
static int l_esp32_getmac(lua_State *L)
{
    int type = luaL_optinteger(L,1,0);
    uint8_t mac[6] = {0};
    esp_read_mac(&mac, type);
    char *macc = (char *)mac;
    lua_pushlstring(L,macc,6);
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
    lua_pushinteger(L,(int)reason);
    return 1;
}

/*
获取随机数
@api esp32.random()
@return number
@usage
esp32.random()
*/
static int l_esp32_random(lua_State *L)
{
    uint32_t r = esp_random();
    lua_pushinteger(L,r);
    return 1;
}

/*
获取chip信息
@api esp32.getchip()
@return table
@usage
local re = esp32.getchip()F
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
	lua_pushinteger(L,info.model);
	lua_settable(L, -3);  

    lua_pushstring(L, "features");
	lua_pushinteger(L,info.features);
	lua_settable(L, -3);  

    lua_pushstring(L, "cores");
	lua_pushinteger(L,info.cores);
	lua_settable(L, -3);  

    lua_pushstring(L, "revision");
	lua_pushinteger(L,info.revision);
	lua_settable(L, -3);  

    return 1;
}


#include "rotable.h"
static const rotable_Reg reg_esp32[] =
    {
        {"getmac",l_esp32_getmac,0},
        {"getRstReason",l_esp32_get_rst_reason,0},
        {"random",l_esp32_random,0},
        {"getchip",l_esp32_get_chip,0},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_esp32(lua_State *L)
{
    luat_newlib(L, reg_esp32);
    return 1;
}
