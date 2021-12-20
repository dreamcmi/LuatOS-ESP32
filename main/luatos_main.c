#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bget.h"
#include "luat_base.h"
#include "luat_msgbus.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_log.h"

// C3两层板可能会重启,先关闭毛刺
#if CONFIG_IDF_TARGET_ESP32C3
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#endif

#ifdef LUAT_USE_LVGL
#include "lvgl.h"
#include "luat_lvgl.h"
#endif

#ifdef CONFIG_SPIRAM
#include "spiram_psram.h"
#endif

#if CONFIG_IDF_TARGET_ESP32C3
#define LUAT_HEAP_SIZE (96 * 1024)
#elif CONFIG_IDF_TARGET_ESP32S3
#define LUAT_HEAP_SIZE (100 * 1024)
#else
#define LUAT_HEAP_SIZE (64 * 1024)
#endif
uint8_t luavm_heap[LUAT_HEAP_SIZE] = {0};

#ifdef LUAT_USE_LVGL

static int luat_lvgl_cb(lua_State *L, void* ptr) {
    lv_task_handler();
    return 0;
}

static void luat_lvgl_callback(TimerHandle_t xTimer) {
    lv_tick_inc(10);
    rtos_msg_t msg = {0};
    msg.handler = luat_lvgl_cb;
    luat_msgbus_put(&msg, 0);
}
#endif

void app_main(void)
{
#ifdef LUAT_USE_LVGL
    lv_init();
#endif

#ifdef CONFIG_SPIRAM
    psram_size_t t = psram_get_size();
    switch (t)
    {
    case 0:
        ESP_LOGW("InitPSRAM", "The chip has 16MBITS PSRAM");
        bpool(heap_caps_malloc(1 * 1024 * 1024, MALLOC_CAP_SPIRAM), 1 * 1024 * 1024);
        break;
    case 1:
        ESP_LOGW("InitPSRAM", "The chip has 32MBITS PSRAM");
        bpool(heap_caps_malloc(3 * 1024 * 1024, MALLOC_CAP_SPIRAM), 3 * 1024 * 1024);
        break;
    case 2:
        ESP_LOGW("InitPSRAM", "The chip has 64MBITS PSRAM");
        bpool(heap_caps_malloc(6 * 1024 * 1024, MALLOC_CAP_SPIRAM), 6 * 1024 * 1024);
        break;
    default: // 到这里就是初始化失败了呗
        ESP_LOGW("InitPSRAM", "DEFAULT:The chip has no PSRAM ");
        bpool(luavm_heap, LUAT_HEAP_SIZE);
        break;
    }
#else
    ESP_LOGE("InitPSRAM", "NOT Support");
    bpool(luavm_heap, LUAT_HEAP_SIZE);
#endif

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

#ifdef LUAT_USE_LVGL
    lv_init();
    TimerHandle_t os_timer;
    os_timer = xTimerCreate("lvgl", 10 / portTICK_RATE_MS, true, NULL, luat_lvgl_callback);
    xTimerStart(os_timer, 0);
#endif

    luat_main();
}

