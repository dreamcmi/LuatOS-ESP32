#include "luat_base.h"
#include "luat_uart.h"
#include "luat_log.h"
#define LUAT_LOG_TAG "luat.uart"

#include "driver/uart.h"
#include "esp_log.h"

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
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    uart_config.source_clk = UART_SCLK_APB,

    uart_driver_install(uart->id, uart->bufsz, 0, 0, NULL, 0);
    uart_param_config(uart->id, &uart_config);

    switch (uart->id)
    {
    case 1:
        uart_set_pin(1, 7, 6, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        break;
    default:
        ESP_LOGE("UART","UARTID:%d not found",uart->id);
        return -1;
        break;
    }
    return 0;
}

int luat_uart_write(int uartid, void *data, size_t length)
{
    if (luat_uart_exist(uartid))
    {
        int err = uart_write_bytes(uartid, (const char *)data, length);
        if (err == -1) return -1;
        else return 0;
    }
    else return -1;
}

int luat_uart_read(int uartid, void *buffer, size_t length)
{
    if (luat_uart_exist(uartid))
    {
        int err = uart_read_bytes(uartid, buffer, length, 20 / portTICK_RATE_MS);
        if (err == -1) return -1;
        else return 0;
    }
    else return -1;
}

int luat_uart_close(int uartid)
{
    if (luat_uart_exist(uartid))
    {
        uart_driver_delete(uartid);
        return 0;
    }
    else return -1;
}

int luat_uart_exist(int uartid)
{
    if (uartid == 1) return 1;
    else
    {
        ESP_LOGE("UART", "uart%d not exist", uartid);
        return 0;
    }
}

int luat_setup_cb(int uartid, int received, int sent)
{
    return 0;
}
