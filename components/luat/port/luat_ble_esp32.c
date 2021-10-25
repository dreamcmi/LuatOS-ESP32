#include "luat_base.h"
#include "luat_ble_esp32.h"
#include "luat_log.h"
#define LUAT_LOG_TAG "luat.ble"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"

#include "string.h"

static int l_ble_init(lua_State *L)
{
    esp_err_t ret = 0;
    ret = esp_bluedroid_init();
    if (ret)
    {
        ESP_LOGE(LOG_TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        lua_pushinteger(L, ret);
    }
    ret = esp_bluedroid_enable();
    if (ret)
    {
        ESP_LOGE(LOG_TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        lua_pushinteger(L, ret);
    }
    lua_pushinteger(L, 1);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_ble[] =
    {
        {"ble_init", l_ble_init, 0},
        {NULL, NULL, 0}};
LUAMOD_API int luaopen_ble(lua_State *L)
{
    luat_newlib(L, reg_ble);
    return 1;
}