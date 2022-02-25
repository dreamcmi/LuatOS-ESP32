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
初始化SD卡与FAT文件系统
@api sdmmc.init(type,clk(cs),cmd,d0)
@int 模式 1=sdio,2=spi
@int clk引脚号 spi模式下cs引脚
@int cmd引脚号
@int d0引脚号
@return int   返回esp_err
@usage
-- 使用sdio挂载sd卡
sdmmc.init(1,12,18,19)
-- 使用spi挂载sd卡
sdmmc.init(2,8,-1,-1)
*/
int l_sdmmc_init(lua_State *L){
	ESP_LOGI(TAG,"Mounting FAT filesystem");
	esp_err_t ret = ESP_FAIL;
	int type = luaL_checkinteger(L,1);
	int clk = luaL_checkinteger(L,2);
	int cmd = luaL_checkinteger(L,3);
	int d0 = luaL_checkinteger(L,4);
	// Mount path for the partition
	sdmmc_card_t *mount_card = NULL;
	const char *base_path = "/sdcard0";
	ESP_LOGI(TAG, "Initializing SDMMC\n");
	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
			.format_if_mount_failed = false,
			.max_files = 5,
	};
	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	switch (type)
	{
		case 1:
#if CONFIG_IDF_TARGET_ESP32C3
			ESP_LOGE(TAG, "C3 does not support SDIO");
			lua_pushboolean(L,false);
			return 1;
#else
			ESP_LOGI(TAG, "Using SDIO Interface");
			sdmmc_slot_config_t sdmmc_slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
			sdmmc_slot_config.clk = clk;
			sdmmc_slot_config.cmd = cmd;
			sdmmc_slot_config.d0 = d0;
			sdmmc_slot_config.width = 1;
			ESP_LOGI(TAG, "Mounting filesystem");
			ret = esp_vfs_fat_sdmmc_mount(base_path, &host, &sdmmc_slot_config, &mount_config, &mount_card);
#endif
			break;
		case 2:
			ESP_LOGI(TAG, "Using SPI Interface");
			host.slot = SDSPI_DEFAULT_HOST;
			//TODO:判断是否初始化过SPI，实现多从机SPI
			spi_bus_config_t bus_cfg = {
					.mosi_io_num = _C3_SPI2_MOSI,
					.miso_io_num = _C3_SPI2_MISO,
					.sclk_io_num = _C3_SPI2_SCLK,
					.quadwp_io_num = -1,
					.quadhd_io_num = -1,
					.max_transfer_sz = 4000,
			};
			sdspi_device_config_t sdspi_slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
			sdspi_slot_config.gpio_cs = clk;
			sdspi_slot_config.host_id = host.slot;
			ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
			if (ret != ESP_OK) {
				ESP_LOGE(TAG, "Failed to initialize bus.");
				lua_pushboolean(L,false);
				return 1;
			}
			ESP_LOGI(TAG, "Mounting filesystem");
			ret = esp_vfs_fat_sdspi_mount(base_path, &host, &sdspi_slot_config, &mount_config, &mount_card);
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
@return int   返回esp_err
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
				{NULL, NULL, 0}};

LUAMOD_API int luaopen_sdmmc(lua_State *L)
{
	luat_newlib(L, reg_sdmmc);
	return 1;
}