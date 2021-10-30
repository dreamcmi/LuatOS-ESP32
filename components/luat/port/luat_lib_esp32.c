#include "luat_base.h"
#include "esp_log.h"
#include "esp_system.h"
#include <string.h>

static int l_esp32_getmac(lua_State *L)
{
    int type = luaL_optinteger(L,1,0);
    uint8_t mac[6] = {0};
    esp_read_mac(&mac, type);
    char *macc = (char *)mac;
    lua_pushlstring(L,macc,6);
    return 1;
}


#include "rotable.h"
static const rotable_Reg reg_esp32[] =
    {
        {"getmac",l_esp32_getmac,0},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_esp32(lua_State *L)
{
    luat_newlib(L, reg_esp32);
    return 1;
}
