/*
 * SPDX-FileCopyrightText: 2021-2022 Fantasy <1005313176@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  sdmmc
@summary esp32_sdmmc操作库
@version 1.0
@date    2022.3.1
*/

#include "luat_base.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "driver/spi_common.h"
#include "driver/sdspi_host.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_defs.h"
#include "driver/sdmmc_types.h"
#include "pinmap.h"


static const char *TAG = "SDMMC";
/*
初始化SD卡与FAT文件系统 默认使用SDSPI 20Mhz
@api sdmmc.init(type,freq,width(cs),clk,cmd,d0(,d1,d2,d3))
@int 模式 1=sdio,2=spi C3仅支持SDSPI模式，S3两种均可使用
@int 频率 SDMMC_FREQ_DEFAULT 20M，SDMMC_FREQ_HIGHSPEED 40M，
@int 宽度 只支持1bit和4bit 仅适用于sdio模式 在sdspi模式下用于设置cs引脚
@int clk引脚号
@int cmd引脚号
@int d0引脚号
@int d1引脚号 当宽度为4bit可用
@int d2引脚号 当宽度为4bit可用
@int d3引脚号 当宽度为4bit可用
@return boolean 成功true 失败false
@usage
-- 使用sdio挂载sd卡
sdmmc.init(1,sdmmc.SDMMC_FREQ_DEFAULT,1,13,12,11)
-- 使用spi挂载sd卡
sdmmc.init(2,sdmmc.SDMMC_FREQ_DEFAULT,4)
*/
int l_sdmmc_init(lua_State *L){
	ESP_LOGI(TAG,"Mounting FAT filesystem");
	esp_err_t ret = ESP_FAIL;
	int type = luaL_checkinteger(L,1);
	int freq = luaL_checkinteger(L,2);
	int width = luaL_checkinteger(L,3);
#ifndef CONFIG_IDF_TARGET_ESP32C3
	int clk = luaL_optinteger(L,4,-1);
	int cmd = luaL_optinteger(L,5,-1);
	int d0 = luaL_optinteger(L,6,-1);
	int d1 = luaL_optinteger(L,7,-1);
	int d2 = luaL_optinteger(L,8,-1);
	int d3 = luaL_optinteger(L,9,-1);
#endif
	sdmmc_card_t *mount_card = NULL;
	const char *base_path = "/sd";
	ESP_LOGI(TAG, "Initializing SDMMC\n");
	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
			.format_if_mount_failed = false,
			.max_files = 5,
	};
	switch (type)
	{
		case 1:
#if CONFIG_IDF_TARGET_ESP32C3
			ESP_LOGE(TAG, "C3 does not support SDIO");
			lua_pushboolean(L,false);
			return 1;
#else
			ESP_LOGI(TAG, "Using SDIO Interface");
			sdmmc_host_t sdmmc_host = SDMMC_HOST_DEFAULT();
			sdmmc_host.max_freq_khz = freq;
			sdmmc_slot_config_t sdmmc_slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
			sdmmc_slot_config.clk = clk;
			sdmmc_slot_config.cmd = cmd;
			sdmmc_slot_config.d0 = d0;
			if(width == 4){
				sdmmc_slot_config.d1 = d1;
				sdmmc_slot_config.d2 = d2;
				sdmmc_slot_config.d3 = d3;
			}
			sdmmc_slot_config.width = width;
			ESP_LOGI(TAG, "Mounting filesystem");
			ret = esp_vfs_fat_sdmmc_mount(base_path, &sdmmc_host, &sdmmc_slot_config, &mount_config, &mount_card);
#endif
			break;
		case 2:
			ESP_LOGI(TAG, "Using SPI Interface");
			sdmmc_host_t sdspi_host = SDSPI_HOST_DEFAULT();
			sdspi_host.max_freq_khz = freq;
			//TODO:判断是否初始化过SPI，并实现多从机SPI
			spi_bus_config_t bus_cfg = {
#if CONFIG_IDF_TARGET_ESP32C3
					.mosi_io_num = _C3_SPI2_MOSI,
					.miso_io_num = _C3_SPI2_MISO,
					.sclk_io_num = _C3_SPI2_SCLK,
#else
					.mosi_io_num = _S3_SPI2_MOSI,
					.miso_io_num = _S3_SPI2_MISO,
					.sclk_io_num = _S3_SPI2_SCLK,
#endif
					.quadwp_io_num = -1,
					.quadhd_io_num = -1,
					.max_transfer_sz = 4000,
			};
			sdspi_device_config_t sdspi_slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
			sdspi_slot_config.gpio_cs = width;
			sdspi_slot_config.host_id = sdspi_host.slot;
			ret = spi_bus_initialize(sdspi_host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
			if (ret != ESP_OK) {
				ESP_LOGE(TAG, "Failed to initialize bus.");
				lua_pushboolean(L,false);
				return 1;
			}
			ESP_LOGI(TAG, "Mounting filesystem");
			ret = esp_vfs_fat_sdspi_mount(base_path, &sdspi_host, &sdspi_slot_config, &mount_config, &mount_card);
			break;
		default:
			ESP_LOGE(TAG, "sdcard type error,mount fail");
			lua_pushboolean(L,false);
			return 1;
	}
	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount filesystem. ");
		} else {
			ESP_LOGE(TAG, "Failed to initialize the card (%s). "
			              "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
		}
		lua_pushboolean(L,false);
		return 1;
	}
	ESP_LOGI(TAG, "Filesystem mounted");
	sdmmc_card_print_info(stdout, mount_card);
	lua_pushboolean(L,true);
	return 1;
}

/*
卸载sd卡文件系统
@api sdmmc.deinit()
@return boolean 成功true 失败false
@usage
-- 卸载sd卡
sdmmc.deinit()
*/
int l_sdmmc_deinit(lua_State *L){
	if (esp_vfs_fat_sdmmc_unmount() == ESP_OK)
	{
		ESP_LOGI(TAG, "Filesystem unmounted");
		lua_pushboolean(L,true);
	}
	else
	{
		ESP_LOGE(TAG, "Filesystem unmount fail");
		lua_pushboolean(L,false);
	}
	return 1;
}

#include "rotable.h"
static const rotable_Reg reg_sdmmc[] =
		{
				{"init", l_sdmmc_init, 0},
				{"deinit", l_sdmmc_deinit, 0},

				{"SDMMC_FREQ_DEFAULT", NULL, SDMMC_FREQ_DEFAULT},
				{"SDMMC_FREQ_HIGHSPEED", NULL, SDMMC_FREQ_HIGHSPEED},

				{NULL, NULL, 0}};

LUAMOD_API int luaopen_sdmmc(lua_State *L)
{
	luat_newlib(L, reg_sdmmc);
	return 1;
}