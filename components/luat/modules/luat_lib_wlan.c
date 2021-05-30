/*
@module  wlan
@summary wifi操作库
@version 1.0
@date    2020.03.30
*/
#include "luat_base.h"

static int l_wlan_get_mode(lua_State *L) {
    ESP_LOGI("l_wlan_get_mode");
    return 1;
}


#include "rotable.h"
static const rotable_Reg reg_wlan[] =
{
    { "getMode" ,  l_wlan_get_mode , 0},
    
	{ NULL, NULL , 0}
};

LUAMOD_API int luaopen_wlan( lua_State *L ) {
    luat_newlib(L, reg_wlan);
    return 1;
}

