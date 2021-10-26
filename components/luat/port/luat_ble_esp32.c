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
    esp_err_t ret = ESP_FAIL;

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    ret = esp_bt_controller_init(&bt_cfg);
    ESP_ERROR_CHECK(ret);

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    ESP_ERROR_CHECK(ret);

    ret = esp_bluedroid_init();
    ESP_ERROR_CHECK(ret);

    ret = esp_bluedroid_enable();
    ESP_ERROR_CHECK(ret);

    lua_pushinteger(L, 1);
    return 1;
}

static int l_ble_deinit(lua_State *L)
{
    esp_err_t ret = ESP_FAIL;

    ret = esp_bluedroid_disable();
    ESP_ERROR_CHECK(ret);

    ret = esp_bluedroid_deinit();
    ESP_ERROR_CHECK(ret);

    ret = esp_bt_controller_disable();
    ESP_ERROR_CHECK(ret);

    ret = esp_bt_controller_deinit();
    ESP_ERROR_CHECK(ret);

    lua_pushinteger(L, 1);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_ble[] =
    {
        {"init", l_ble_init, 0},
        {"deinit",l_ble_deinit,0},
        {NULL, NULL, 0}};
LUAMOD_API int luaopen_ble(lua_State *L)
{
    luat_newlib(L, reg_ble);
    return 1;
}