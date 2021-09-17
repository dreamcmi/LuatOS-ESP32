// #include "luat_base.h"
// #include "luat_spi.h"
// #include "luat_log.h"
// #include "esp_log.h"
// #include "driver/spi_master.h"
// #include "driver/gpio.h"

// #define LUAT_LOG_TAG "luat.spi"
// spi_device_handle_t spi_h;
// //TODO:待优化，目前存在多个警告
// //十六进制字符串转uint8_t
// uint8_t convert(char * src)
// {
// 	luat_log_log(1,LUAT_LOG_TAG," hex str: %s\n",src);
// 	char* src0;
// 	uint8_t ret;
// 	ret = strtol(src,&src0,16);
// 	luat_log_log(1,LUAT_LOG_TAG,"ret: %x\n",ret);
// 	return ret;
// }

// //初始化配置SPI各项参数，并打开SPI,向SPI总线添加设备
// int luat_spi_setup(luat_spi_t *spi)
// {
// 	// SPI2_HOST = 1
// 	// SPI3_HOST = 2
// 	if (spi->id == 1)
// 	{
// 		spi_bus_config_t spi_config = {};
// 		spi_config.miso_io_num = 12;
// 		spi_config.mosi_io_num = 13;
// 		spi_config.sclk_io_num = 14;
// 		spi_config.quadwp_io_num = -1;
// 		spi_config.quadhd_io_num = -1;
// 		spi_config.max_transfer_sz = 0;
// 		ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &spi_config, 0));
// 	} else if (spi->id == 2)
// 	{
// 		spi_bus_config_t spi_config = {};
// 		spi_config.miso_io_num = 19;
// 		spi_config.mosi_io_num = 23;
// 		spi_config.sclk_io_num = 18;
// 		spi_config.quadwp_io_num = -1;
// 		spi_config.quadhd_io_num = -1;
// 		spi_config.max_transfer_sz = 0;
// 		ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &spi_config, 0));
// 	} else
// 	{
// 		return -1;
// 	}
// 	spi_device_interface_config_t devcfg = {};
// 	memset(&devcfg,0,sizeof(devcfg));
// 	if (spi->CPHA == 0)
// 	{
// 		if (spi->CPOL == 0)
// 		{
// 			devcfg.mode = 0;
// 		} else if (spi->CPOL == 1)
// 		{
// 			devcfg.mode = 1;
// 		}
// 	}
// 	if (spi->CPHA == 1)
// 	{
// 		if (spi->CPOL == 0)
// 		{
// 			devcfg.mode = 2;
// 		} else if (spi->CPOL == 1)
// 		{
// 			devcfg.mode = 3;
// 		}
// 	}

// 	if (spi->bit_dict == 2)
// 	{
// 		devcfg.flags = SPI_DEVICE_BIT_LSBFIRST;
// 	}
// 	devcfg.cs_ena_pretrans = 1;
// 	devcfg.clock_speed_hz = spi->bandrate;
// 	luat_log_log(1, LUAT_LOG_TAG, "CS:%d\n", spi->cs);
// 	//默认无 值255
// 	if (spi->cs == 255)
// 		spi->cs = spi->id == 1 ? 15 : 5;
// 	devcfg.spics_io_num = spi->cs;
// 	devcfg.input_delay_ns = 0;
// 	devcfg.queue_size = 6;
// 	if (spi_bus_add_device(spi->id, &devcfg, &spi_h) != ESP_OK)
// 		return -1;
// 	gpio_set_level(spi->cs, 1);
// 	return 0;
// }

// //关闭SPI，成功返回0
// int luat_spi_close(int spi_id)
// {
// 	if (spi_id == 1 || spi_id == 2)
// 	{
// 		ESP_ERROR_CHECK(spi_bus_remove_device(spi_h));
// 		ESP_ERROR_CHECK(spi_bus_free(spi_id));
// 		return 0;
// 	}
// 	return -1;
// }
// //TODO:收发函数报错，确认为136行 待修复
// //收发SPI数据，通过recv_buf返回接收字符串
// int luat_spi_transfer(int spi_id, const char *send_buf, char *recv_buf, size_t length)
// {
// 	if (spi_id == 1 || spi_id == 2)
// 	{
// 		spi_transaction_t t;
// 		memset(&t,0,sizeof(t));
// 		uint8_t hsend_buf = convert(send_buf);
// 		if(hsend_buf == 0)
// 			return -1;
// 		t.length = sizeof(uint8_t) * 8;
// 		t.tx_buffer = &hsend_buf;
// 		t.rx_buffer = recv_buf;
// 		t.rxlength = 0;
// 		esp_err_t ret = spi_device_transmit(spi_h, &t);
// 		luat_log_log(1,LUAT_LOG_TAG,"SPI recv error check: %d\n",ret);
// 		if (ret == ESP_OK)
// 			return 0;
// 	}
// 	return -1;
// }
// //TODO:收函数报错，确认为154行 待修复
// //收SPI数据，通过recv_buf返回接收字符串
// int luat_spi_recv(int spi_id, char *recv_buf, size_t length)
// {
// 	if (spi_id == 1 || spi_id == 2)
// 	{
// 		spi_transaction_t t;
// 		memset(&t,0,sizeof(t));
// 		t.length = sizeof(uint8_t) * 8;
// 		t.rx_buffer = recv_buf;
// 		t.tx_buffer = NULL;
// 		esp_err_t ret = spi_device_transmit(spi_h, &t);
// 		luat_log_log(1,LUAT_LOG_TAG,"SPI recv error check: %d\n",ret);
// 		if (ret == ESP_OK)
// 			return 0;
// 	}
// 	return -1;
// }
// //TODO:发送不报错，有待确认是否成功发送
// //发SPI数据，返回发送结果
// int luat_spi_send(int spi_id, const char *send_buf, size_t length)
// {
// 	if (spi_id == 1 || spi_id == 2)
// 	{
// 		spi_transaction_t t;
// 		memset(&t,0,sizeof(t));
// 		uint8_t hsend_buf = convert(send_buf);
// 		t.length = sizeof(uint8_t) * 8;
// 		t.tx_buffer = &hsend_buf;
// 		esp_err_t ret = spi_device_transmit(spi_h, &t);
// 		luat_log_log(1,LUAT_LOG_TAG,"SPI recv error check: %d\n",ret);
// 		if(ret == ESP_OK)
// 			return 0;
// 	}
// 	return -1;
// }
