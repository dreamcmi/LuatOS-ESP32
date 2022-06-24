#include <stdio.h>

#include "sdkconfig.h"
#include "luat_conf_bsp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_idf_version.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include "bget.h"
#include "luat_base.h"
#include "luat_msgbus.h"
#include "luat_gpio.h"
#include "luat_uart.h"
#include "luat_shell.h"
#include "luat_cmux.h"

#ifdef LUAT_USE_LVGL
#include "lvgl.h"
#include "luat_lvgl.h"
#endif

#ifdef CONFIG_SPIRAM
#include "spiram_psram.h"
#endif

#if CONFIG_IDF_TARGET_ESP32C3
#if LUAT_USE_ESPSSL
#define LUAT_HEAP_SIZE (80 * 1024) // ssl占用较大,缩小16klua heap
#else
#define LUAT_HEAP_SIZE (96 * 1024)
#endif
#elif CONFIG_IDF_TARGET_ESP32S3
#define LUAT_HEAP_SIZE (100 * 1024)
#else
#define LUAT_HEAP_SIZE (64 * 1024)
#endif
uint8_t luavm_heap[LUAT_HEAP_SIZE] = {0};

xQueueHandle gpio_evt_queue = NULL;

#ifdef LUAT_USE_LVGL
static int luat_lvgl_cb(lua_State *L, void *ptr)
{
    lv_task_handler();
    return 0;
}

static void luat_lvgl_callback(TimerHandle_t xTimer)
{
    lv_tick_inc(10);
    rtos_msg_t msg = {0};
    msg.handler = luat_lvgl_cb;
    luat_msgbus_put(&msg, 0);
}
#endif

static void gpio_irq_task(void *arg)
{
    uint32_t io_num = 0;
    int pin_level = 0;
    rtos_msg_t msg = {0};
    while (true)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            // printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
            pin_level = gpio_get_level(io_num);
            // msg.handler = l_gpio_handler;
            // msg.ptr = NULL;
            // msg.arg1 = io_num;
            // msg.arg2 = pin_level;
            // luat_msgbus_put(&msg, 0);
            luat_gpio_irq_default(io_num, pin_level);
        }
    }
    vTaskDelete(NULL);
}

xQueueHandle uart0_evt_queue = NULL;
#define UART0_BUFFER_SIZE 1024
static char uart0_buffer[UART0_BUFFER_SIZE] = {0};
static void uart0_irq_task(void *arg)
{
    uart_event_t event = {0};
#ifndef LUAT_USE_SHELL
    rtos_msg_t msg = {0};
#endif
#ifdef LUAT_USE_SHELL
    int len = 0;
#endif
    while (true)
    {
        if (xQueueReceive(uart0_evt_queue, (void *)&event, (portTickType)portMAX_DELAY))
        {
            if (event.timeout_flag || event.size > (1024 * 2 - 200))
            {
#ifdef LUAT_USE_SHELL
                //memset(uart0_buffer, 0, UART0_BUFFER_SIZE);
                len = uart_read_bytes(0, uart0_buffer, UART0_BUFFER_SIZE - 1, 10 / portTICK_RATE_MS);
                if (len < 1)
                    continue;
                // for (size_t i = 0; i < len; i++){
                //     LLOGD("buffer[%d]:0x%02X",i,buffer[i]);
                // }
                uart0_buffer[len] = 0x00; // 确保结尾
                luat_shell_push(uart0_buffer, len);
#else
                msg.handler = l_uart_handler;
                msg.ptr = NULL;
                msg.arg1 = 0; // uart0
                msg.arg2 = 1; // recv
                luat_msgbus_put(&msg, 0);
#endif
                xQueueReset(uart0_evt_queue);
            }
        }
    }
    vTaskDelete(NULL);
}

TaskHandle_t luatosHandle = NULL;
void luat_shell_poweron(void) ;

void run_luat_main(void* args) {
    luat_main();
}

void app_main(void)
{
    // 如果使能debug,需要高一点的波特率
    uart_set_baudrate(0, 921600);
    // uart0是log口,早开一下中断会不会更好呢？？
    uart_driver_install(0, 1024 * 2, 1024 * 2, 20, &uart0_evt_queue, 0);
    uart_pattern_queue_reset(0, 20);
    xTaskCreate(uart0_irq_task, "uart0_irq_task", 2048, NULL, 10, NULL);

    uint8_t *mac = malloc(10);
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    printf("\nMac:%02x%02x%02x%02x%02x%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    free(mac);
    printf("IDF_VERSION:%d.%d.%d\n", ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH);

#ifdef ESP_BETA_VERSION
    printf("The current version is in beta mode, please do not use it in a production environment!\n");
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

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_irq_task, "gpio_irq_task", 2048, NULL, 10, NULL);

#ifdef LUAT_USE_LVGL
    lv_init();
    TimerHandle_t os_timer;
    os_timer = xTimerCreate("lvgl", 10 / portTICK_RATE_MS, true, NULL, luat_lvgl_callback);
    xTimerStart(os_timer, 0);
#endif
#ifdef CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    luat_shell_poweron();
#endif
    xTaskCreate(run_luat_main, "luat_main", 12*1024, NULL, 2, &luatosHandle);
    // luat_main();
}

#ifdef CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
int usb_serial_jtag_read_bytes(void* buf, uint32_t length, TickType_t ticks_to_wait);
static char* shell_tmp_buff[1024];
void luat_shell_thread_entry(void* args) {
    int ret = 0;
    while (1) {
        ret = usb_serial_jtag_read_bytes(shell_tmp_buff, 512, 1000);
        if (ret > 0) {
            luat_shell_push(shell_tmp_buff, ret);
        }
    }
}

void luat_shell_poweron(void) {
    // xTaskCreate(luat_shell_thread_entry, "luat_shell", 1024, NULL, 2, NULL);
}
#endif
