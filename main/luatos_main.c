#include <stdio.h>
// #include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bget.h"
#include "luat_base.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#define LUAT_HEAP_SIZE (64*1024)
uint8_t luavm_heap[LUAT_HEAP_SIZE] = {0};


void app_main(void)
{
    bpool(luavm_heap, LUAT_HEAP_SIZE);  

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    // ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    // esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    // esp_bt_controller_init(&bt_cfg);
    // esp_bt_controller_enable(ESP_BT_MODE_BLE);
    
    luat_main();     
}
