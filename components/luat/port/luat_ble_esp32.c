#include "luat_base.h"
#include "luat_ble_esp32.h"
#include "luat_log.h"
#define LUAT_LOG_TAG "luat.ble"

#include "esp_log.h"
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

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    ret = esp_bluedroid_init();
    if (ret)
    {
        ESP_LOGE("LBLE", "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        lua_pushinteger(L, ret);
    }
    else
    {
        ret = esp_bluedroid_enable();
        if (ret)
        {
            ESP_LOGE("LBLE", "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
            lua_pushinteger(L, ret);
        }
    }
    lua_pushinteger(L, 1);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_ble[] =
    {
        {"init", l_ble_init, 0},
        {NULL, NULL, 0}};
LUAMOD_API int luaopen_ble(lua_State *L)
{
    luat_newlib(L, reg_ble);
    return 1;
}