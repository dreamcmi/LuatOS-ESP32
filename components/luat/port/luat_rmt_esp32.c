#include "luat_base.h"
#include "esp_log.h"
#include "driver/rmt.h"


static int l_rmt_init(lua_State *L)
{
    int rmtio = luaL_checkinteger(L, 1);
    int rmtch = luaL_checkinteger(L, 2);
    uint8_t rmtdiv = luaL_optinteger(L, 3, 80);
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(rmtio, rmtch);
    config.clk_div = rmtdiv;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
    lua_pushinteger(L, 1);
    return 1;
}


static int l_rmt_deinit(lua_State *L)
{
    int rmtch = luaL_checkinteger(L, 1);
    ESP_ERROR_CHECK(rmt_driver_uninstall(rmtch));
    lua_pushinteger(L, 1);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_rmt[] =
    {
        {"init", l_rmt_init, 0},
        {"deinit", l_rmt_deinit, 0},
        {NULL, NULL, 0}
    };

LUAMOD_API int luaopen_rmt(lua_State *L)
{
    luat_newlib(L, reg_rmt);
    return 1;
}