/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"
#include "luat_gpio.h"
#include "luat_spi.h"
#include "luat_malloc.h"

#include "pinmap.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define LUAT_LOG_TAG "spi"
#include "luat_log.h"

static spi_device_handle_t spi2_handle = {0};

#if CONFIG_IDF_TARGET_ESP32S3
static spi_device_handle_t spi3_handle = {0};
#endif

int luat_spi_setup(luat_spi_t *spi)
{
    esp_err_t ret = -1;
    if (spi->id == 2)
    {
        spi_bus_config_t buscfg = {
#if CONFIG_IDF_TARGET_ESP32C3
            .miso_io_num = _C3_SPI2_MISO,
            .mosi_io_num = _C3_SPI2_MOSI,
            .sclk_io_num = _C3_SPI2_SCLK,
#elif CONFIG_IDF_TARGET_ESP32S3
            .miso_io_num = _S3_SPI2_MISO,
            .mosi_io_num = _S3_SPI2_MOSI,
            .sclk_io_num = _S3_SPI2_SCLK,
#endif
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE
        };
        ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
        if (ret != 0)
        {
            return ret;
        }
        spi_device_interface_config_t dev_config;
        memset(&dev_config, 0, sizeof(dev_config));
        if (spi->CPHA == 0)
        {
            if (spi->CPOL == 0)
                dev_config.mode = 0;
            if (spi->CPOL == 1)
                dev_config.mode = 1;
        }
        else
        {
            if (spi->CPOL == 0)
                dev_config.mode = 2;
            if (spi->CPOL == 1)
                dev_config.mode = 3;
        }
        dev_config.clock_speed_hz = spi->bandrate;
        dev_config.spics_io_num = -1; //用户自行控制cs
        dev_config.queue_size = 7;
        ret = spi_bus_add_device(SPI2_HOST, &dev_config, &spi2_handle);
        return ret;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (spi->id == 3)
    {
        spi_bus_config_t buscfg = {
            .miso_io_num = _S3_SPI3_MISO,
            .mosi_io_num = _S3_SPI3_MOSI,
            .sclk_io_num = _S3_SPI3_SCLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE};
        ret = spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO);
        if (ret != 0)
        {
            return ret;
        }
        spi_device_interface_config_t dev_config;
        memset(&dev_config, 0, sizeof(dev_config));
        if (spi->CPHA == 0)
        {
            if (spi->CPOL == 0)
                dev_config.mode = 0;
            if (spi->CPOL == 1)
                dev_config.mode = 1;
        }
        else
        {
            if (spi->CPOL == 0)
                dev_config.mode = 2;
            if (spi->CPOL == 1)
                dev_config.mode = 3;
        }
        dev_config.clock_speed_hz = spi->bandrate;
        dev_config.spics_io_num = -1; //用户自行控制cs
        dev_config.queue_size = 7;
        ret = spi_bus_add_device(SPI3_HOST, &dev_config, &spi3_handle);
        return ret;
    }
#endif
    else
        return -1;
}

int luat_spi_close(int spi_id)
{
    esp_err_t ret = -1;
    if (spi_id == 2)
    {
        ret = spi_bus_remove_device(spi2_handle);
        if (ret != 0)
        {
            return ret;
        }
        ret = spi_bus_free(SPI2_HOST);
        return ret;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (spi_id == 3)
    {
        spi_bus_remove_device(spi3_handle);
        if (ret != 0)
        {
            return ret;
        }
        spi_bus_free(SPI3_HOST);
        return ret;
    }
#endif
    else
        return -1;
}

int luat_spi_transfer(int spi_id, const char *send_buf, size_t send_length, char *recv_buf, size_t recv_length)
{
    esp_err_t ret = -1;
    if (spi_id == 2)
    {
        /* FIX:一句执行发送指令不满4会自动补满,导致多发00 */
        spi_transaction_t send;
        memset(&send, 0, sizeof(send));
        send.length = send_length * 8;
        send.tx_buffer = send_buf;
        ret = spi_device_polling_transmit(spi2_handle, &send);
        if (ret != 0)
        {
            return -2;
        }
        spi_transaction_t recv;
        memset(&recv, 0, sizeof(recv));
        recv.length = recv_length * 8;
        recv.rxlength = recv_length * 8;
        recv.rx_buffer = recv_buf;
        ret = spi_device_polling_transmit(spi2_handle, &recv);
        return ret == 0 ? recv_length : -1;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (spi_id == 3)
    {
        /* FIX:一句执行发送指令不满4会自动补满,导致多发00 */
        spi_transaction_t send;
        memset(&send, 0, sizeof(send));
        send.length = send_length * 8;
        send.tx_buffer = send_buf;
        ret = spi_device_polling_transmit(spi3_handle, &send);
        if (ret != 0)
        {
            return -2;
        }
        spi_transaction_t recv;
        memset(&recv, 0, sizeof(recv));
        recv.length = recv_length * 8;
        recv.rxlength = recv_length * 8;
        recv.rx_buffer = recv_buf;
        ret = spi_device_polling_transmit(spi3_handle, &recv);
        return ret == 0 ? recv_length : -1;
    }
#endif
    else
        return -1;
}

int luat_spi_recv(int spi_id, char *recv_buf, size_t length)
{
    esp_err_t ret = -1;
    if (spi_id == 2)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.rxlength = length * 8;
        t.rx_buffer = recv_buf;
        ret = spi_device_polling_transmit(spi2_handle, &t);
        return ret == 0 ? length : -1;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (spi_id == 3)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.rxlength = length * 8;
        t.rx_buffer = recv_buf;
        ret = spi_device_polling_transmit(spi3_handle, &t);
        return ret == 0 ? length : -1;
    }
#endif
    else
        return -1;
}

int luat_spi_send(int spi_id, const char *send_buf, size_t length)
{
    spi_transaction_t t;
    esp_err_t ret = -1;
    if (spi_id == 2)
    {
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.tx_buffer = send_buf;
        ret = spi_device_polling_transmit(spi2_handle, &t);
        return ret == 0 ? length : -1;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (spi_id == 3)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.tx_buffer = send_buf;
        ret = spi_device_polling_transmit(spi3_handle, &t);
        return ret == 0 ? length : -1;
    }
#endif
    else
        return -1;
}

#define LUAT_SPI_CS_SELECT 0
#define LUAT_SPI_CS_CLEAR 1

static uint8_t spi_bus2 = 0;
#if CONFIG_IDF_TARGET_ESP32S3
static uint8_t spi_bus3 = 0;
#endif

// luat_spi_device_t* 在lua层看到的是一个userdata
int luat_spi_device_setup(luat_spi_device_t *spi_dev)
{
    esp_err_t ret = -1;
    int bus_id = spi_dev->bus_id;
    spi_device_handle_t *spi_device = luat_heap_malloc(sizeof(spi_device_handle_t));
    if (spi_device == NULL)
        return ret;
    spi_dev->user_data = (void *)spi_device;
    if (bus_id == 2 && spi_bus2 == 0)
    {
        spi_bus_config_t buscfg = {
#if CONFIG_IDF_TARGET_ESP32C3
            .miso_io_num = _C3_SPI2_MISO,
            .mosi_io_num = _C3_SPI2_MOSI,
            .sclk_io_num = _C3_SPI2_SCLK,
#elif CONFIG_IDF_TARGET_ESP32S3
            .miso_io_num = _S3_SPI2_MISO,
            .mosi_io_num = _S3_SPI2_MOSI,
            .sclk_io_num = _S3_SPI2_SCLK,
#endif
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4092 * 2
        };
        ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
        if (ret != 0)
        {
            return ret;
        }
        spi_bus2 = 1;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (bus_id == 3 && spi_bus3 == 0)
    {
        spi_bus_config_t buscfg = {
            .miso_io_num = _S3_SPI3_MISO,
            .mosi_io_num = _S3_SPI3_MOSI,
            .sclk_io_num = _S3_SPI3_SCLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4092 * 2};
        ret = spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO);
        if (ret != 0)
        {
            return ret;
        }
        spi_bus3 = 1;
    }
#endif
    spi_device_interface_config_t dev_config;
    memset(&dev_config, 0, sizeof(dev_config));
    if (spi_dev->spi_config.CPHA == 0)
    {
        if (spi_dev->spi_config.CPOL == 0)
            dev_config.mode = 0;
        if (spi_dev->spi_config.CPOL == 1)
            dev_config.mode = 1;
    }
    else
    {
        if (spi_dev->spi_config.CPOL == 0)
            dev_config.mode = 2;
        if (spi_dev->spi_config.CPOL == 1)
            dev_config.mode = 3;
    }
    dev_config.clock_speed_hz = spi_dev->spi_config.bandrate;
    dev_config.spics_io_num = -1; //用户自行控制cs
    dev_config.queue_size = 7;
    if (spi_dev->spi_config.mode == 0)
        dev_config.flags = SPI_DEVICE_HALFDUPLEX;
    if (bus_id == 2)
        ret = spi_bus_add_device(SPI2_HOST, &dev_config, spi_device);
#if CONFIG_IDF_TARGET_ESP32S3
    else if (bus_id == 3)
        ret = spi_bus_add_device(SPI3_HOST, &dev_config, spi_device);
#endif
    if (ret != 0)
        luat_heap_free(spi_device);
    luat_gpio_mode(spi_dev->spi_config.cs, Luat_GPIO_OUTPUT, Luat_GPIO_DEFAULT, Luat_GPIO_HIGH); // CS
    return ret;
}

//关闭SPI设备，成功返回0
int luat_spi_device_close(luat_spi_device_t *spi_dev)
{
    esp_err_t ret = -1;
    int bus_id = spi_dev->bus_id;
    if (bus_id == 2 || bus_id == 3)
    {
        ret = spi_bus_remove_device(*(spi_device_handle_t *)(spi_dev->user_data));
    }
    luat_heap_free((spi_device_handle_t *)(spi_dev->user_data));
    return ret;
}

//收发SPI数据，返回接收字节数
int luat_spi_device_transfer(luat_spi_device_t *spi_dev, const char *send_buf, size_t send_length, char *recv_buf, size_t recv_length)
{
    luat_gpio_set(spi_dev->spi_config.cs, LUAT_SPI_CS_SELECT);
    esp_err_t ret = -1;
    int bus_id = spi_dev->bus_id;
    if (bus_id == 2 || bus_id == 3)
    {
        spi_transaction_t send;
        memset(&send, 0, sizeof(send));
        send.length = send_length * 8;
        send.tx_buffer = send_buf;
        ret = spi_device_polling_transmit(*(spi_device_handle_t *)(spi_dev->user_data), &send);
        if (ret != 0)
        {
            return -2;
        }
        spi_transaction_t recv;
        memset(&recv, 0, sizeof(recv));
        recv.length = recv_length * 8;
        recv.rxlength = recv_length * 8;
        recv.rx_buffer = recv_buf;
        ret = spi_device_polling_transmit(*(spi_device_handle_t *)(spi_dev->user_data), &recv);
    }
    luat_gpio_set(spi_dev->spi_config.cs, LUAT_SPI_CS_CLEAR);
    return ret == 0 ? recv_length : -1;
}

//收SPI数据，返回接收字节数
int luat_spi_device_recv(luat_spi_device_t *spi_dev, char *recv_buf, size_t length)
{
    luat_gpio_set(spi_dev->spi_config.cs, LUAT_SPI_CS_SELECT);
    esp_err_t ret = -1;
    int bus_id = spi_dev->bus_id;
    if (bus_id == 2 || bus_id == 3)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.rxlength = length * 8;
        t.rx_buffer = recv_buf;
        ret = spi_device_polling_transmit(*(spi_device_handle_t *)(spi_dev->user_data), &t);
    }
    luat_gpio_set(spi_dev->spi_config.cs, LUAT_SPI_CS_CLEAR);
    return ret == 0 ? length : -1;
}

//发SPI数据，返回发送字节数
int luat_spi_device_send(luat_spi_device_t *spi_dev, const char *send_buf, size_t length)
{
    luat_gpio_set(spi_dev->spi_config.cs, LUAT_SPI_CS_SELECT);
    esp_err_t ret = -1;
    int bus_id = spi_dev->bus_id;
    if (bus_id == 2 || bus_id == 3)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.tx_buffer = send_buf;
        ret = spi_device_polling_transmit(*(spi_device_handle_t *)(spi_dev->user_data), &t);
    }
    luat_gpio_set(spi_dev->spi_config.cs, LUAT_SPI_CS_CLEAR);
    return ret == 0 ? length : -1;
}
