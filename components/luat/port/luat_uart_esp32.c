#include "luat_base.h"
#include "luat_uart.h"
#include "luat_log.h"
#define LUAT_LOG_TAG "luat.uart"

#include "driver/uart.h"
// #include "esp_log.h"
// static const char *TAG = "uart_events";

// #define BUF_SIZE (1024)
// #define RD_BUF_SIZE (BUF_SIZE)
// #define EX_UART_NUM UART_NUM_0
// #define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

// static void uart_callback()
// {
//     uart_event_t event;
//     size_t buffered_size;
//     uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE);
//     //Waiting for UART event.
//     bzero(dtmp, RD_BUF_SIZE);
//     ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);
//     switch (event.type)
//     {
//     //Event of UART receving data
//     /*We'd better handler data event fast, there would be much more data events than
//                 other types of events. If we take too much time on data event, the queue might
//                 be full.*/
//     case UART_DATA:
//         ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
//         uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
//         ESP_LOGI(TAG, "[DATA EVT]:");
//         uart_write_bytes(EX_UART_NUM, (const char *)dtmp, event.size);
//         break;
//     //Event of HW FIFO overflow detected
//     case UART_FIFO_OVF:
//         ESP_LOGI(TAG, "hw fifo overflow");
//         // If fifo overflow happened, you should consider adding flow control for your application.
//         // The ISR has already reset the rx FIFO,
//         // As an example, we directly flush the rx buffer here in order to read more data.
//         uart_flush_input(EX_UART_NUM);
//         break;
//     //Event of UART ring buffer full
//     case UART_BUFFER_FULL:
//         ESP_LOGI(TAG, "ring buffer full");
//         // If buffer full happened, you should consider encreasing your buffer size
//         // As an example, we directly flush the rx buffer here in order to read more data.
//         uart_flush_input(EX_UART_NUM);
//         break;
//     //Event of UART RX break detected
//     case UART_BREAK:
//         ESP_LOGI(TAG, "uart rx break");
//         break;
//     //Event of UART parity check error
//     case UART_PARITY_ERR:
//         ESP_LOGI(TAG, "uart parity error");
//         break;
//     //Event of UART frame error
//     case UART_FRAME_ERR:
//         ESP_LOGI(TAG, "uart frame error");
//         break;
//     //UART_PATTERN_DET
//     case UART_PATTERN_DET:
//         uart_get_buffered_data_len(EX_UART_NUM, &buffered_size);
//         int pos = uart_pattern_pop_pos(EX_UART_NUM);
//         ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
//         if (pos == -1)
//         {
//             // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
//             // record the position. We should set a larger queue size.
//             // As an example, we directly flush the rx buffer here.
//             uart_flush_input(EX_UART_NUM);
//         }
//         else
//         {
//             uart_read_bytes(EX_UART_NUM, dtmp, pos, 100 / portTICK_PERIOD_MS);
//             uint8_t pat[PATTERN_CHR_NUM + 1];
//             memset(pat, 0, sizeof(pat));
//             uart_read_bytes(EX_UART_NUM, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
//             ESP_LOGI(TAG, "read data: %s", dtmp);
//             ESP_LOGI(TAG, "read pat : %s", pat);
//         }
//         break;
//     //Others
//     default:
//         ESP_LOGI(TAG, "uart event type: %d", event.type);
//         break;
//     }

//     free(dtmp);
//     dtmp = NULL;
// }

int luat_uart_setup(luat_uart_t *uart)
{
    uart_config_t uart_config = {};
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
        LLOGE("error uart.data_bits");
        return -1;
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
        LLOGE("error uart.parity");
        return -1;
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
        uart_set_pin(uart->id, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        break;
    case 2:
        uart_set_pin(uart->id, 25, 26, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        break;
    default:
        LLOGE("error uart.id");
        return -1;
        break;
    }
    return 0;
}

int luat_uart_write(int uartid, void *data, size_t length)
{
    if (luat_uart_exist(uartid))
    {
        uart_write_bytes(uartid, (const char *)data, length);
        return 0;
    }
    else
    {
        return -1;
    }
}

int luat_uart_read(int uartid, void *buffer, size_t length)
{
    if (luat_uart_exist(uartid))
    {
        uart_read_bytes(uartid, buffer, length, 20 / portTICK_RATE_MS);
        return 0;
    }
    else
    {
        return -1;
    }
}

int luat_uart_close(int uartid)
{
    if (luat_uart_exist(uartid))
    {
        uart_driver_delete(uartid);
        return 0;
    }
    else
    {
        return -1;
    }
}

int luat_uart_exist(int uartid)
{
    if (uartid == 1 || uartid == 2)
    {
        return 1;
    }
    else
    {
        luat_log_warn("luat.uart", "uart%ld not exist", uartid);
        return 0;
    }
}

int luat_setup_cb(int uartid, int received, int sent)
{
    return 0;
}