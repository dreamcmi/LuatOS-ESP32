#include "luat_base.h"
#include "luat_spi.h"
#include "luat_malloc.h"

#include "pinmap.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define LUAT_LOG_TAG "spi"
#include "luat_log.h"

static spi_device_handle_t spi_handle = {0};

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
        ESP_ERROR_CHECK(ret);
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
        ret = spi_bus_add_device(SPI2_HOST, &dev_config, &spi_handle);
        ESP_ERROR_CHECK(ret);
        if (ret == ESP_OK)
            return 0;
        else
            return -1;
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
        ESP_ERROR_CHECK(ret);
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
        ESP_ERROR_CHECK(ret);
        if (ret == ESP_OK)
            return 0;
        else
            return -1;
    }
#endif
    else
        return -1;
}

int luat_spi_close(int spi_id)
{
    if (spi_id == 2)
    {
        ESP_ERROR_CHECK(spi_bus_remove_device(spi_handle));
        ESP_ERROR_CHECK(spi_bus_free(SPI2_HOST));
        return 0;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (spi_id == 3)
    {
        ESP_ERROR_CHECK(spi_bus_remove_device(spi3_handle));
        ESP_ERROR_CHECK(spi_bus_free(SPI3_HOST));
        return 0;
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
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = send_length * 8;
        t.rxlength = recv_length * 8;
        t.tx_buffer = send_buf;
        t.rx_buffer = recv_buf;
        ret = spi_device_polling_transmit(spi_handle, &t);
        ESP_ERROR_CHECK(ret);
        if (ret == ESP_OK)
            return recv_length;
        else
            return -1;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (spi_id == 3)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = send_length * 8;
        t.rxlength = recv_length * 8;
        t.tx_buffer = send_buf;
        t.rx_buffer = recv_buf;
        ret = spi_device_polling_transmit(spi3_handle, &t);
        ESP_ERROR_CHECK(ret);
        if (ret == ESP_OK)
            return recv_length;
        else
            return -1;
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
        t.rxlength = length * 8;
        t.rx_buffer = recv_buf;
        ret = spi_device_polling_transmit(spi_handle, &t);
        ESP_ERROR_CHECK(ret);
        if (ret == ESP_OK)
            return length;
        else
            return -1;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (spi_id == 3)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.rx_buffer = recv_buf;
        ret = spi_device_polling_transmit(spi3_handle, &t);
        ESP_ERROR_CHECK(ret);
        if (ret == ESP_OK)
            return length;
        else
            return -1;
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
        ret = spi_device_polling_transmit(spi_handle, &t);
        ESP_ERROR_CHECK(ret);
        if (ret == ESP_OK)
            return length;
        else
            return -1;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (spi_id == 3)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.tx_buffer = send_buf;
        ret = spi_device_polling_transmit(spi3_handle, &t);
        ESP_ERROR_CHECK(ret);
        if (ret == ESP_OK)
            return length;
        else
            return -1;
    }
#endif
    else
        return -1;
}

// luat_spi_device_t* 在lua层看到的是一个userdata
int luat_spi_device_setup(luat_spi_device_t *spi_dev)
{
    esp_err_t ret = -1;
    int bus_id = spi_dev->bus_id;
    spi_device_handle_t *spi_device = luat_heap_malloc(sizeof(spi_device_handle_t));
    if (spi_device == NULL)
        return ret;
    spi_dev->user_data = (void *)spi_device;
    if (bus_id == 2)
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
        ESP_ERROR_CHECK(ret);
    }
#if CONFIG_IDF_TARGET_ESP32S3
    else if (bus_id == 3)
    {
        spi_bus_config_t buscfg = {
            .miso_io_num = _S3_SPI3_MISO,
            .mosi_io_num = _S3_SPI3_MOSI,
            .sclk_io_num = _S3_SPI3_SCLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4092 * 2};
        ret = spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO);
        ESP_ERROR_CHECK(ret);
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
    dev_config.spics_io_num = spi_dev->spi_config.cs; //用户自行控制cs
    dev_config.queue_size = 7;
    if (bus_id == 2)
        ret = spi_bus_add_device(SPI2_HOST, &dev_config, spi_device);
#if CONFIG_IDF_TARGET_ESP32S3
    else if (bus_id == 3)
        ret = spi_bus_add_device(SPI3_HOST, &dev_config, spi_device);
#endif
    ESP_ERROR_CHECK(ret);
    if (ret != 0)
        luat_heap_free(spi_device);
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
        ESP_ERROR_CHECK(ret);
    }
    luat_heap_free((spi_device_handle_t *)(spi_dev->user_data));
    return ret;
}

//收发SPI数据，返回接收字节数
int luat_spi_device_transfer(luat_spi_device_t *spi_dev, const char *send_buf, size_t send_length, char *recv_buf, size_t recv_length)
{
    esp_err_t ret = -1;
    int bus_id = spi_dev->bus_id;
    if (bus_id == 2 || bus_id == 3)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = send_length * 8;
        t.rxlength = recv_length * 8;
        t.tx_buffer = send_buf;
        t.rx_buffer = recv_buf;
        esp_err_t ret = spi_device_polling_transmit(*(spi_device_handle_t *)(spi_dev->user_data), &t);
        ESP_ERROR_CHECK(ret);
    }
    if (ret == 0)
        return recv_length;
    else
        return -1;
}

//收SPI数据，返回接收字节数
int luat_spi_device_recv(luat_spi_device_t *spi_dev, char *recv_buf, size_t length)
{
    esp_err_t ret = -1;
    int bus_id = spi_dev->bus_id;
    if (bus_id == 2 || bus_id == 3)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.rxlength = length * 8;
        t.rx_buffer = recv_buf;
        esp_err_t ret = spi_device_polling_transmit(*(spi_device_handle_t *)(spi_dev->user_data), &t);
        ESP_ERROR_CHECK(ret);
    }
    if (ret == 0)
        return length;
    else
        return -1;
}

//发SPI数据，返回发送字节数
int luat_spi_device_send(luat_spi_device_t *spi_dev, const char *send_buf, size_t length)
{
    esp_err_t ret = -1;
    int bus_id = spi_dev->bus_id;
    if (bus_id == 2 || bus_id == 3)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.tx_buffer = send_buf;
        ret = spi_device_polling_transmit(*(spi_device_handle_t *)(spi_dev->user_data), &t);
        ESP_ERROR_CHECK(ret);
    }
    if (ret == 0)
        return length;
    else
        return -1;
}
