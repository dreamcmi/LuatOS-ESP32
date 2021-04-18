#include "luat_base.h"
#include "luat_spi.h"

#include "driver/spi_master.h"
#include "driver/spi_common.h"

spi_device_handle_t spi_h;

/*
未验证！
未验证！
未验证！
未验证！
待我再理解下idf
*/

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
        spi_bus_initialize(spi->id, &spi_config, 0);
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
        spi_bus_initialize(spi->id, &spi_config, 0);
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

    spi_bus_add_device(spi->id, &devcfg, &spi_h);

    return 0;
}

//关闭SPI，成功返回0
int luat_spi_close(int spi_id)
{
    if (spi_id == 1 || spi_id == 2)
    {
        spi_bus_remove_device(&spi_h);
        spi_bus_free(spi_id);
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
        send_spi.length = length;
        send_spi.tx_buffer = send_buf;
        send_spi.rx_buffer = recv_buf;
        spi_device_transmit(&spi_h, &send_spi);
        return 0;
    }
    return -1;
}

//收SPI数据，返回接收字节数
int luat_spi_recv(int spi_id, char *recv_buf, size_t length)
{
    return 0;
}

//发SPI数据，返回发送字节数
int luat_spi_send(int spi_id, const char *send_buf, size_t length)
{
    return 0;
}
