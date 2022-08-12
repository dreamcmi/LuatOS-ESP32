/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"
#include "luat_mcu.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "esp_pm.h"
#include "esp_system.h"

#define LUAT_LOG_TAG "mcu"
#include "luat_log.h"

// #define UUID_USE_EFUSE
#define UUID_USE_MAC

#ifdef UUID_USE_EFUSE
#undef UUID_USE_MAC
#include "esp_efuse.h"
#endif

#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/clk.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/clk.h"
#else
#error "luat_mcu_esp32 target error"
#endif

long luat_mcu_ticks(void)
{
    return xTaskGetTickCount();
}

static char uuid[16] = {0};
const char *luat_mcu_unique_id(size_t *t)
{
#ifdef UUID_USE_EFUSE
    *t = 16;
    int uid[16] = {0};
    for (size_t i = 0; i < 16; i++)
    {
        uid[i] = esp_efuse_read_reg(EFUSE_BLK2, i);
    }
    memcpy(uuid, uid, 16);

#else
    *t = 6;
    uint8_t *mac = malloc(10);
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    memcpy(uuid, mac, 6);
    free(mac);
#endif

    return (const char *)uuid;
}

int luat_mcu_set_clk(size_t mhz)
{
    int xtal_freq = rtc_clk_xtal_freq_get();
#if CONFIG_IDF_TARGET_ESP32S3
    esp_pm_config_esp32s3_t pm_config = {
#elif CONFIG_IDF_TARGET_ESP32C3
    esp_pm_config_esp32c3_t pm_config = {
#endif
        .max_freq_mhz = (int)mhz,
        .min_freq_mhz = (mhz < xtal_freq ? mhz : xtal_freq),
    };
    esp_err_t err = esp_pm_configure(&pm_config);
    return err;
}

int luat_mcu_get_clk(void)
{
    return (esp_clk_cpu_freq() / MHZ);
}

uint32_t luat_mcu_hz(void)
{
    return configTICK_RATE_HZ;
}

uint64_t luat_mcu_tick64(void) {
    LLOGE("not support tick64");
    return 0;
}

int luat_mcu_us_period(void) {
    return 1;
}

uint64_t luat_mcu_tick64_ms(void) {
    LLOGE("not support tick64");
    return 0;
}
void luat_mcu_set_clk_source(uint8_t source_main, uint8_t source_32k, uint32_t delay) {
    LLOGE("not support setXTAL");
}

