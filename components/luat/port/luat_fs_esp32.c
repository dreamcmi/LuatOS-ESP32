/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_fs.h"
#include "luat_msgbus.h"
#include "luat_ota.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_partition.h"

#ifdef LUAT_USE_LVGL
#include "lvgl.h"
#include "luat_lvgl.h"
#include "lv_sjpg.h"
#endif

static const char *TAG = "LSPIFFS";

#ifdef LUAT_USE_FS_VFS
extern const struct luat_vfs_filesystem vfs_fs_posix;
#ifdef LUAT_USE_SFUD
extern const struct luat_vfs_filesystem vfs_fs_lfs2;
#endif
#ifdef LUAT_USE_VFS_INLINE_LIB
extern const char luadb_inline_sys[];
extern const struct luat_vfs_filesystem vfs_fs_luadb;
#endif
#endif

static const void *map_ptr;
static spi_flash_mmap_handle_t map_handle;
static const esp_partition_t * partition;
// 文件系统初始化函数
esp_vfs_spiffs_conf_t spiffs_conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 10,
    .format_if_mount_failed = true};

int luat_fs_init(void)
{
  printf("Initializing SPIFFS\n");

  esp_err_t ret = esp_vfs_spiffs_register(&spiffs_conf);

  if (ret != ESP_OK)
  {
    if (ret == ESP_FAIL)
    {
      ESP_LOGE(TAG, "Failed to mount or format filesystem\n");
    }
    else if (ret == ESP_ERR_NOT_FOUND)
    {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition\n");
    }
    else
    {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret));
    }
    return 1;
  }
  size_t total = 0, used = 0;
  ret = esp_spiffs_info(spiffs_conf.partition_label, &total, &used);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
  }
  else
  {
    printf("Partition size: total: %d, used: %d\n", total, used);
  }
#ifdef LUAT_USE_FS_VFS
#ifdef LUAT_USE_SFUD
  luat_vfs_reg(&vfs_fs_lfs2);
#endif
  luat_vfs_reg(&vfs_fs_posix);
	luat_fs_conf_t conf = {
		.busname = "",
		.type = "posix",
		.filesystem = "posix",
		.mount_point = ""
	};
	luat_fs_mount(&conf);
	
  // 先注册luadb
	luat_vfs_reg(&vfs_fs_luadb);
  // 默认指向内部luadb
	luat_fs_conf_t conf2 = {
		.busname = (char*)luadb_inline_sys,
		.type = "luadb",
		.filesystem = "luadb",
		.mount_point = "/luadb/",
	};
  // 寻找脚本分区,存在就用, 不存在就算了, 兼容老的
  // TODO 支持OTA, 通过切换分区的方式
  partition = esp_partition_find_first(0x5A, 0x5A, "script");
  if (partition != NULL) {
    ESP_LOGI("vfs", "found script partition %08X %08X", partition->address, (uint32_t)luat_vfs_reg);
#ifdef LUAT_USE_OTA
    //OTA检测升级
    luat_ota(0);
#endif
    esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &map_ptr, &map_handle);
    conf2.busname = (char*)map_ptr;
    if (luat_fs_mount(&conf2)) {
      // 挂载失败, 回退到内嵌luadb
      conf2.busname = (char*)luadb_inline_sys;
      luat_fs_mount(&conf2);
    }
  }
  else {
    ESP_LOGE("vfs", "script partition not found");
    luat_fs_mount(&conf2);

#ifdef LUAT_USE_LVGL
	luat_lv_fs_init();
	// lv_bmp_init();
	// lv_png_init();
	lv_split_jpeg_init();
#endif
  }
	
#endif

  return 0;
}

// int luat_fs_info(const char *path, luat_fs_info_t *conf)
// {
//   size_t total = 0, used = 0;
//   if (strcmp("/spiffs", path))
//   {
//     ESP_LOGE(TAG,"not support path %s", path);
//     return -2;
//   }
//   esp_err_t ret = esp_spiffs_info(spiffs_conf.partition_label, &total, &used);
//   if (ret != ESP_OK)
//   {
//     ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
//     return -1;
//   }
//   else
//   {
//     conf->type = 1; // 片上FLASH
//     conf->total_block = total;
//     conf->block_used = used;
//     conf->block_size = 4096;
//     memcpy(conf->filesystem, "spiffs", 6);
//     conf->filesystem[7] = 0;
//   }
//   return 0;
// }

#ifdef LUAT_USE_OTA
int luat_flash_write(uint32_t addr, uint8_t * buf, uint32_t len){
  // printf("addr %x %d\r\n", addr,len);
  esp_partition_erase_range(partition,addr,len*sizeof(uint8_t));
  return esp_partition_write(partition,addr, buf, len*sizeof(uint8_t));
}
#endif
