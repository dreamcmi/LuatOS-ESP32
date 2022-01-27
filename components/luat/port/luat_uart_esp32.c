#include "luat_base.h"
#include "luat_uart.h"
#include "luat_shell.h"
#include "luat_log.h"
#define LUAT_LOG_TAG "luat.uart"

#include "pinmap.h"
#include "driver/uart.h"
#include "esp_log.h"
static const char *TAG = "LUART";
// static xQueueHandle uart0_evt_queue = NULL;
static xQueueHandle uart1_evt_queue = NULL;
static xQueueHandle uart2_evt_queue = NULL;

// static void uart0_irq_task(void *arg)
// {
//     uart_event_t event = {0};
//     rtos_msg_t msg = {0};
//     char buffer[1024] = {0};
//     int len = 0;
//     while (true)
//     {
//         if (xQueueReceive(uart0_evt_queue, (void *)&event, (portTickType)portMAX_DELAY))
//         {
//             switch (event.type)
//             {
//             case UART_DATA:
//                 msg.handler = l_uart_handler;
//                 msg.ptr = NULL;
//                 msg.arg1 = 0; //uart1
//                 msg.arg2 = 1; //recv
//                 luat_msgbus_put(&msg, 0);
//                 len = uart_read_bytes(0, buffer, 1024, 10 / portTICK_RATE_MS);
//                 luat_shell_push(buffer, len);
//                 xQueueReset(uart0_evt_queue);
//                 break;
//             default:
//                 // ESP_LOGE("uart", "uart1 event type: %d", event.type);
//                 break;
//             }
//         }
//     }
//     vTaskDelete(NULL);
// }

static void uart1_irq_task(void *arg)
{
    uart_event_t event = {0};
    rtos_msg_t msg = {0};
    while (true)
    {
        if (xQueueReceive(uart1_evt_queue, (void *)&event, (portTickType)portMAX_DELAY))
        {
            if (event.timeout_flag || event.size > (1024 * 2 - 200))
            {
                // printf("uart1 data\n");
                msg.handler = l_uart_handler;
                msg.ptr = NULL;
                msg.arg1 = 1; //uart1
                msg.arg2 = 1; //recv
                luat_msgbus_put(&msg, 0);
                xQueueReset(uart1_evt_queue);
            }
        }
    }
    vTaskDelete(NULL);
}

static void uart2_irq_task(void *arg)
{
    uart_event_t event = {0};
    rtos_msg_t msg = {0};
    while (true)
    {
        if (xQueueReceive(uart2_evt_queue, (void *)&event, (portTickType)portMAX_DELAY))
        {
            if (event.timeout_flag || event.size > (1024 * 2 - 200))
            {
                msg.handler = l_uart_handler;
                msg.ptr = NULL;
                msg.arg1 = 2; //uart2
                msg.arg2 = 1; //recv
                luat_msgbus_put(&msg, 0);
                xQueueReset(uart2_evt_queue);
            }
        }
    }
    vTaskDelete(NULL);
}

int luat_uart_setup(luat_uart_t *uart)
{
    uart_config_t uart_config = {0};
    uart_config.baud_rate = uart->baud_rate;
    switch (uart->data_bits)
    {
    case 8:
        uart_config.data_bits = UART_DATA_8_BITS;
        break;
    case 7:
        uart_config.data_bits = UART_DATA_7_BITS;
        break;
    default:
        uart_config.data_bits = UART_DATA_8_BITS;
        break;
    }

    switch (uart->parity)
    {
    case LUAT_PARITY_NONE:
        uart_config.parity = UART_PARITY_DISABLE;
        break;
    case LUAT_PARITY_ODD:
        uart_config.parity = UART_PARITY_ODD;
        break;
    case LUAT_PARITY_EVEN:
        uart_config.parity = UART_PARITY_EVEN;
        break;
    default:
        uart_config.parity = UART_PARITY_DISABLE;
        break;
    }

    uart_config.stop_bits = uart->stop_bits;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_APB;

    switch (uart->id)
    {
    case 0:
        // uart_driver_install(0, uart->bufsz, 1024 * 2, 20, &uart0_evt_queue, 0);
        break;
    case 1:
        uart_driver_install(1, uart->bufsz, 1024 * 2, 20, &uart1_evt_queue, 0);
        break;
    case 2:
        uart_driver_install(2, uart->bufsz, 1024 * 2, 20, &uart2_evt_queue, 0);
        break;
    default:
        break;
    }
    uart_param_config(uart->id, &uart_config);
    uart_pattern_queue_reset(uart->id, 20);
    switch (uart->id)
    {
    case 0:
        // xTaskCreate(uart0_irq_task, "uart0_irq_task", 4096, NULL, 10, NULL);
        break;
    case 1:
        xTaskCreate(uart1_irq_task, "uart1_irq_task", 4096, NULL, 10, NULL);
        break;
    case 2:
        xTaskCreate(uart2_irq_task, "uart2_irq_task", 4096, NULL, 10, NULL);
        break;
    default:
        break;
    }
    switch (uart->id)
    {
    case 1:
#if CONFIG_IDF_TARGET_ESP32C3
        uart_set_pin(1, _C3_U1TX, _C3_U1RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        break;
#elif CONFIG_IDF_TARGET_ESP32S3
        uart_set_pin(1, _S3_U1TX, _S3_U1RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        break;
    case 2:
        uart_set_pin(2, _S3_U2TX, _S3_U2RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        break;
#endif
    default:
        ESP_LOGE(TAG, "UARTID:%d not found", uart->id);
        return -1;
        break;
    }
    return 0;
}

int luat_uart_write(int uartid, void *data, size_t length)
{
    if (luat_uart_exist(uartid))
    {
        int len = uart_write_bytes(uartid, (const char *)data, length);
        return len;
    }
    else
        return -1;
}

void luat_shell_write(char *buff, size_t len)
{
    uart_write_bytes(0, (const char *)buff, len);
}

int luat_uart_read(int uartid, void *buffer, size_t length)
{
    if (luat_uart_exist(uartid))
    {
        int len = uart_read_bytes(uartid, buffer, length, 10 / portTICK_RATE_MS);
        return len;
    }
    else
        return -1;
}

int luat_uart_close(int uartid)
{
    if (luat_uart_exist(uartid))
    {
        uart_driver_delete(uartid);
        return 0;
    }
    else
        return -1;
}

int luat_uart_exist(int uartid)
{
    if (uartid < SOC_UART_NUM)
        return 1;
    else
        return 0;
}

int luat_setup_cb(int uartid, int received, int sent)
{
    return 0;
}
