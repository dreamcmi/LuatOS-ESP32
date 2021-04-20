#include "luat_base.h"
#include "luat_spi.h"
#define LUAT_LOG_TAG "luat.spi"
#include "luat_log.h"

#include "driver/spi_master.h"
#include "driver/spi_common.h"

spi_device_handle_t spi_h;


//初始化配置SPI各项参数，并打开SPI
//成功返回0
int luat_spi_setup(luat_spi_t *spi)
{
    // SPI2_HOST = 1
    // SPI3_HOST = 2
    if (spi->id == 1)
    {
        spi_bus_config_t spi_config = {
            .miso_io_num = 12,
            .mosi_io_num = 13,
            .sclk_io_num = 14,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 0,
        };
        switch (spi_bus_initialize(spi->id, &spi_config, 0))
        {
            case ESP_ERR_INVALID_ARG:
                LLOGE(LUAT_LOG_TAG,"SPI Init Bus Fail,Invalid Arg");
                return -1;
                break;
            case ESP_ERR_INVALID_STATE:
                LLOGE(LUAT_LOG_TAG,"SPI Init Bus Fail,Invalid State");
                return -1;
                break;
            case ESP_ERR_NO_MEM:
                LLOGE(LUAT_LOG_TAG,"SPI Init Bus Fail,No Memory");
                return -1;
                break;
        }
    }
    else if (spi->id == 2)
    {
        spi_bus_config_t spi_config = {
            .miso_io_num = 19,
            .mosi_io_num = 23,
            .sclk_io_num = 18,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 0,
        };
        switch (spi_bus_initialize(spi->id, &spi_config, 0))
        {
            case ESP_ERR_INVALID_ARG:
                LLOGE(LUAT_LOG_TAG,"SPI Init Bus Fail,Invalid Arg");
                return -1;
                break;
            case ESP_ERR_INVALID_STATE:
                LLOGE(LUAT_LOG_TAG,"SPI Init Bus Fail,Invalid State");
                return -1;
                break;
            case ESP_ERR_NO_MEM:
                LLOGE(LUAT_LOG_TAG,"SPI Init Bus Fail,No Memory");
                return -1;
                break;
        }
    }
    else
    {
        return -1;
    }

    spi_device_interface_config_t devcfg;

    if (spi->CPHA == 0)
    {
        if (spi->CPOL == 0)
        {
            devcfg.mode = 0;
        }
        else if (spi->CPOL == 1)
        {
            devcfg.mode = 1;
        }
    }
    if (spi->CPHA == 1)
    {
        if (spi->CPOL == 0)
        {
            devcfg.mode = 2;
        }
        else if (spi->CPOL == 1)
        {
            devcfg.mode = 3;
        }
    }

    if (spi->bit_dict == 2)
    {
        devcfg.flags = SPI_DEVICE_BIT_LSBFIRST;
    }

    devcfg.command_bits = 8;
    devcfg.address_bits = 8;
    devcfg.duty_cycle_pos = 0;
    devcfg.clock_speed_hz = spi->bandrate;
    devcfg.spics_io_num = spi->cs;
    devcfg.input_delay_ns = 0;

    switch (spi_bus_add_device(spi->id, &devcfg, &spi_h))
    {
    case ESP_ERR_INVALID_ARG:
        LLOGE(LUAT_LOG_TAG,"SPI Bus Add Device Fail,Invalid Arg");
        return -1;
        break;
    case ESP_ERR_NOT_FOUND:
        LLOGE(LUAT_LOG_TAG,"SPI Bus Add Device Fail,Invalid Arg");
        return -1;
        break;
    case ESP_ERR_NO_MEM:
        LLOGE(LUAT_LOG_TAG,"SPI Bus Add Device Fail,No Memory");
        return -1;
        break;
    }

    return 0;
}

//关闭SPI，成功返回0
int luat_spi_close(int spi_id)
{
    if (spi_id == 1 || spi_id == 2)
    {
        switch (spi_bus_remove_device(spi_h))
        {
        case ESP_ERR_INVALID_ARG:
            LLOGE(LUAT_LOG_TAG,"SPI Bus Remove Device Fail,Invalid Arg");
            return -1;
            break;
        case ESP_ERR_INVALID_STATE:
            LLOGE(LUAT_LOG_TAG,"SPI Bus Remove Device Fail,Invalid State");
            return -1;
            break;
        }
        switch (spi_bus_free(spi_id))
        {
        case ESP_ERR_INVALID_ARG:
            LLOGE(LUAT_LOG_TAG,"SPI Bus Free Fail,Invalid Arg");
            return -1;
            break;

        case ESP_ERR_INVALID_STATE:
            LLOGE(LUAT_LOG_TAG,"SPI Bus Free Device Fail,Invalid State");
            return -1;
            break;
        }
        return 0;
    }
    return -1;
}

//收发SPI数据，返回接收字节数
int luat_spi_transfer(int spi_id, const char *send_buf, char *recv_buf, size_t length)
{
    if (spi_id == 1 || spi_id == 2)
    {
        spi_transaction_t send_spi;
        // send_spi.length = length;
        send_spi.tx_buffer = send_buf;
        send_spi.rx_buffer = recv_buf;
        send_spi.rxlength = length;
        switch (spi_device_transmit(spi_h, &send_spi))
        {
        case ESP_OK:
            return 0;
            break;
        case ESP_ERR_INVALID_ARG:
            LLOGE(LUAT_LOG_TAG,"SPI Recv Fail,Invalid Arg");
            return -1;
            break;
        }
        return 0;
    }
    return -1;
}

//收SPI数据，返回接收字节数
int luat_spi_recv(int spi_id, char *recv_buf, size_t length)
{
    if (spi_id == 1 || spi_id == 2)
    {
        spi_transaction_t send_spi;
        send_spi.rxlength = length;
        send_spi.rx_buffer = recv_buf;
        send_spi.tx_buffer = NULL;
        switch (spi_device_transmit(spi_h, &send_spi))
        {
        case ESP_OK:
            return 0;
            break;
        case ESP_ERR_INVALID_ARG:
            LLOGE(LUAT_LOG_TAG,"SPI Recv Fail,Invalid Arg");
            return -1;
            break;
        }
    }
    return -1;
}

//发SPI数据，返回发送字节数
int luat_spi_send(int spi_id, const char *send_buf, size_t length)
{
    if (spi_id == 1 || spi_id == 2)
    {
        spi_transaction_t send_spi;
        send_spi.length = length;
        send_spi.tx_buffer = send_buf;
        send_spi.rx_buffer = NULL;
        switch (spi_device_transmit(spi_h, &send_spi))
        {
        case ESP_OK:
            return 0;
            break;
        case ESP_ERR_INVALID_ARG:
            LLOGE(LUAT_LOG_TAG,"SPI Recv Fail,Invalid Arg");
            return -1;
            break;
        }
    }
    return -1;
}
