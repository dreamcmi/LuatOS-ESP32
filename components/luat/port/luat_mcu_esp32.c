#include "luat_base.h"
#include "luat_mcu.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "esp_system.h"
// #include "esp_efuse.h"
#include "sdkconfig.h"

long luat_mcu_ticks(void)
{
    return xTaskGetTickCount();
}

static char uuid[6] = {0};
const char *luat_mcu_unique_id(size_t *t)
{
    *t = 6;
    /* 可以从efuse读取uid,参考以下方法
    // char *uid[128] = {0};
    // for (size_t i = 0; i < 128; i++)
    // {
    //     uid[i] = esp_efuse_read_reg(EFUSE_BLK2, i);
    // }
    // memcpy(uuid, uid, 128);
    */
    uint8_t *mac = malloc(10);
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    memcpy(uuid, mac, 6);
    free(mac);

    return (const char *)&uuid;
}

int luat_mcu_set_clk(size_t mhz)
{
    return 0;
}

int luat_mcu_get_clk(void)
{
#if CONFIG_IDF_TARGET_ESP32C3
    return CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ;
#elif CONFIG_IDF_TARGET_ESP32S3
    return CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ;
#else
    return 0;
#endif
}