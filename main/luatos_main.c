#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bget.h"
#include "luat_base.h"

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

void app_main(void)
{
    // C3两层板可能会重启,先关闭毛刺
#if CONFIG_IDF_TARGET_ESP32C3
    REG_SET_FIELD(RTC_CNTL_FIB_SEL_REG, RTC_CNTL_FIB_SEL, RTC_CNTL_FIB_SUPER_WDT_RST | RTC_CNTL_FIB_BOR_RST);
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

    luat_main();
}
