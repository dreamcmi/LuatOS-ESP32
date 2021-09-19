#include "luat_base.h"
#include "luat_spi.h"
#include "luat_log.h"
#include "esp_log.h"
#include "driver/spi_master.h"

#define LUAT_LOG_TAG "luat.spi"
spi_device_handle_t spi_h;
//十六进制字符串转uint8_t
uint8_t AsciiToHex8(const char * src)
{
	luat_log_log(1,LUAT_LOG_TAG," hex str: %s\n",src);
	char* src0;
	uint8_t ret;
	ret = strtol(src,&src0,16);
	luat_log_log(1,LUAT_LOG_TAG,"ret: %x\n",ret);
	return ret;
}
//十六进制字符串转uint16_t
uint16_t AsciiToHex16(const char * src)
{
	luat_log_log(1,LUAT_LOG_TAG," hex str: %s\n",src);
	char* src0;
	uint16_t ret;
	ret = strtol(src,&src0,16);
	luat_log_log(1,LUAT_LOG_TAG,"ret: %x\n",ret);
	return ret;
}
//十六进制字符串转uint32_t
uint32_t AsciiToHex32(const char * src)
{
	luat_log_log(1,LUAT_LOG_TAG," hex str: %s\n",src);
	char* src0;
	uint32_t ret;
	ret = strtol(src,&src0,16);
	luat_log_log(1,LUAT_LOG_TAG,"ret: %x\n",ret);
	return ret;
}
//十六进制转字符串
void HexToAscii(uint8_t *src, char *dest, int len)
{
	char dh,dl;  //字符串的高位和低位
	int i;
	if(src == NULL || dest == NULL)
	{
		printf("src or dest is NULL\n");
		return;
	}
	if(len < 1)
	{
		printf("length is NULL\n");
		return;
	}
	for(i = 0; i < len; i++)
	{
		dh = '0' + src[i] / 16;
		dl = '0' + src[i] % 16;
		if(dh > '9')
		{
			dh = dh - '9' - 1 + 'A'; // 或者 dh= dh+ 7;
		}
		if(dl > '9')
		{
			dl = dl - '9' - 1 + 'A'; // 或者dl = dl + 7;
		}
		dest[2*i] = dh;
		dest[2*i+1] = dl;
	}
	dest[2*i] = '\0';
}
//初始化配置SPI各项参数，并打开SPI,向SPI总线添加设备
int luat_spi_setup(luat_spi_t *spi)
{
	// SPI2_HOST = 1
	// SPI3_HOST = 2
	if (spi->id == 1)
	{
		spi_bus_config_t spi_config = {};
		spi_config.miso_io_num = 12;
		spi_config.mosi_io_num = 13;
		spi_config.sclk_io_num = 14;
		spi_config.quadwp_io_num = -1;
		spi_config.quadhd_io_num = -1;
		spi_config.max_transfer_sz = 0;
		ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &spi_config, 0));
	} else if (spi->id == 2)
	{
		spi_bus_config_t spi_config = {};
		spi_config.miso_io_num = 19;
		spi_config.mosi_io_num = 23;
		spi_config.sclk_io_num = 18;
		spi_config.quadwp_io_num = -1;
		spi_config.quadhd_io_num = -1;
		spi_config.max_transfer_sz = 0;
		ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &spi_config, 0));
	} else
	{
		return -1;
	}
	spi_device_interface_config_t devcfg = {};
	memset(&devcfg,0,sizeof(devcfg));
	if (spi->CPHA == 0)
	{
		if (spi->CPOL == 0)
		{
			devcfg.mode = 0;
		} else if (spi->CPOL == 1)
		{
			devcfg.mode = 1;
		}
	}
	if (spi->CPHA == 1)
	{
		if (spi->CPOL == 0)
		{
			devcfg.mode = 2;
		} else if (spi->CPOL == 1)
		{
			devcfg.mode = 3;
		}
	}

	if (spi->bit_dict == 2)
	{
		devcfg.flags = SPI_DEVICE_BIT_LSBFIRST;
	}
	//跳过命令和地址阶段
	devcfg.command_bits = 0;
	devcfg.address_bits = 0;
	devcfg.cs_ena_pretrans = 1;
	devcfg.clock_speed_hz = spi->bandrate;
	devcfg.queue_size = 6;
	//CS默认无 值255
	if (spi->cs == 255) spi->cs = spi->id == 1 ? 15 : 5;
	devcfg.spics_io_num = spi->cs;
	if (spi_bus_add_device(spi->id, &devcfg, &spi_h) != ESP_OK)
		return -1;
	return 0;
}

//关闭SPI，成功返回0
int luat_spi_close(int spi_id)
{
	if (spi_id == 1 || spi_id == 2)
	{
		ESP_ERROR_CHECK(spi_bus_remove_device(spi_h));
		ESP_ERROR_CHECK(spi_bus_free(spi_id));
		return 0;
	}
	return -1;
}
//收发SPI数据，通过recv_buf返回接收字符串
int luat_spi_transfer(int spi_id, const char *send_buf, char *recv_buf, size_t length)
{
	if(length*8 > 32)
	{
		luat_log_error(LUAT_LOG_TAG,"Data exceeds 32 bits");
		return -1;
	}
	if(length == 0)
	{
		luat_log_error(LUAT_LOG_TAG,"Data length can't be 0");
		return -1;
	}
	if (spi_id == 1 || spi_id == 2)
	{
		uint8_t *hex_recv8_buf = NULL;
		uint16_t *hex_recv16_buf = NULL;
		uint32_t *hex_recv32_buf = NULL;
		spi_transaction_t t;
		memset(&t,0,sizeof(t));
		luat_log_debug(LUAT_LOG_TAG,"Data is %d byte\n",length);
		t.length = length*8;
		if (length == 1)
		{
			uint8_t hex_send_buf = AsciiToHex8(send_buf);
			luat_log_debug(LUAT_LOG_TAG,"AsciiToHex8 %x\n",hex_send_buf);
			t.tx_buffer = &hex_send_buf;
		}
		else if (length == 2)
		{
			uint16_t hex_send_buf = AsciiToHex16(send_buf);
			luat_log_debug(LUAT_LOG_TAG,"AsciiToHex16 %x\n",hex_send_buf);
			t.tx_buffer = &hex_send_buf;
		}
		else if (length == 4)
		{
			uint32_t hex_send_buf = AsciiToHex32(send_buf);
			luat_log_debug(LUAT_LOG_TAG,"AsciiToHex32 %x\n",hex_send_buf);
			t.tx_buffer = &hex_send_buf;
		}
		else
		{
			luat_log_error(LUAT_LOG_TAG,"Data greater than 32 bits\n");
			return -1;
		}
		if (length == 1)
		{
			luat_log_debug(LUAT_LOG_TAG,"Recv Hex8\n");
			t.rx_buffer = hex_recv8_buf;
		}
		else if (length == 2)
		{
			luat_log_debug(LUAT_LOG_TAG,"Recv Hex16\n");
			t.rx_buffer = hex_recv16_buf;
		}
		else if (length == 4)
		{
			luat_log_debug(LUAT_LOG_TAG,"Recv Hex32\n");
			t.rx_buffer = hex_recv32_buf;
		}
		esp_err_t ret = spi_device_transmit(spi_h, &t);
//		luat_log_debug(LUAT_LOG_TAG,"recv: %d",*recv_buffer);
		if (hex_recv8_buf != NULL)
			HexToAscii(hex_recv8_buf,recv_buf,sizeof(hex_recv8_buf));
//		if ((*hex_recv16_buf)!=0)
//			HexToAscii(hex_recv16_buf,recv_buf,sizeof(hex_recv16_buf));
//		if ((*hex_recv32_buf)!=0)
//			HexToAscii(hex_recv32_buf,recv_buf,sizeof(hex_recv32_buf));
		luat_log_log(1,LUAT_LOG_TAG,"SPI recv error check: %d\n",ret);
		if (ret == ESP_OK)
			return 0;
	}
	return -1;
}
//暂时不用单独收发
//收SPI数据，通过recv_buf返回接收字符串
int luat_spi_recv(int spi_id, char *recv_buf, size_t length)
{
	if (spi_id == 1 || spi_id == 2)
	{
//		spi_transaction_t t;
//		memset(&t,0,sizeof(t));
//		t.length = 8;
//		t.rx_buffer = recv_buf;
//		t.tx_buffer = NULL;
//		esp_err_t ret = spi_device_transmit(spi_h, &t);
//		luat_log_log(1,LUAT_LOG_TAG,"SPI recv error check: %d\n",ret);
//		if (ret == ESP_OK)
//			return 0;
	}
	return -1;
}
//发SPI数据，返回发送结果
int luat_spi_send(int spi_id, const char *send_buf, size_t length)
{
//	if (spi_id == 1 || spi_id == 2)
	{
//		spi_transaction_t t;
//		memset(&t,0,sizeof(t));
//		uint8_t hsend_buf = convert(send_buf);
//		t.length = sizeof(uint8_t) * 8;
//		t.tx_buffer = &hsend_buf;
//		t.rx_buffer = NULL;
//		esp_err_t ret = spi_device_transmit(spi_h, &t);
//		luat_log_log(1,LUAT_LOG_TAG,"SPI send error check: %d\n",ret);
//		if(ret == ESP_OK)
//			return 0;
	}
	return -1;
}
