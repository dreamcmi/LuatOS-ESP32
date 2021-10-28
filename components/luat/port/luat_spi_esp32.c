#include "luat_base.h"
#include "luat_spi.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define LUAT_LOG_TAG "luat.spi"
#include "luat_log.h"

spi_device_handle_t spi_handle = {0};

int luat_spi_setup(luat_spi_t *spi)
{
    esp_err_t err = -1;
    if (spi->id == 2)
    {
        spi_bus_config_t buscfg = {
            .miso_io_num = 6,
            .mosi_io_num = 7,
            .sclk_io_num = 8,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE};
        err = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
        ESP_ERROR_CHECK(err);
        ESP_LOGD("SPI", "bus-err:%d", err);
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
        dev_config.queue_size = 7,
        err = spi_bus_add_device(SPI2_HOST, &dev_config, &spi_handle);
        ESP_ERROR_CHECK(err);
        ESP_LOGD("SPI", "device-err:%d", err);
        if (err == ESP_OK)
            return 0;
        else
            return -1;
    }
    else
        return -1;
}

int luat_spi_close(int spi_id)
{
    esp_err_t err;
    if (spi_id == 2)
    {
        err = spi_bus_remove_device(spi_handle);
        ESP_ERROR_CHECK(err);
        err = spi_bus_free(SPI2_HOST);
        ESP_ERROR_CHECK(err);
        return 0;
    }
    else
        return -1;
}

int luat_spi_transfer(int spi_id, const char *send_buf, char *recv_buf, size_t length)
{
    if (spi_id == 2)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.tx_buffer = send_buf;
        t.rx_buffer = recv_buf;
        esp_err_t err = spi_device_polling_transmit(spi_handle, &t);
        ESP_ERROR_CHECK(err);
        ESP_LOGD("SPI", "trans-err:%d", err);
        if (err == ESP_OK)
            return 0;
        else
            return -1;
    }
    else
        return -1;
}

int luat_spi_recv(int spi_id, char *recv_buf, size_t length)
{
    if (spi_id == 2)
    {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.rx_buffer = recv_buf;
        t.flags = SPI_TRANS_USE_RXDATA;
        esp_err_t err = spi_device_polling_transmit(spi_handle, &t);
        ESP_ERROR_CHECK(err);
        ESP_LOGD("SPI", "recv-err:%d", err);
        if (err == ESP_OK)
            return 0;
        else
            return -1;
    }
    else
        return -1;
}

int luat_spi_send(int spi_id, const char *send_buf, size_t length)
{
    spi_transaction_t t;
    esp_err_t ret;
    if (spi_id == 2)
    {
        memset(&t, 0, sizeof(t));
        t.length = length * 8;
        t.tx_buffer = send_buf;
        ret = spi_device_polling_transmit(spi_handle,&t);
        ESP_ERROR_CHECK(ret);
        ESP_LOGD("SPI", "send-err:%d", ret);
        if (ret == ESP_OK)
            return 0;
        else
            return -1;
    }
    else
        return -1;
}
