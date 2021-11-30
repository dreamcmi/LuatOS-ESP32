#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_fs.h"
#include "luat_msgbus.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "esp_system.h"

static const char *TAG = "LSPIFFS";

#ifdef LUAT_USE_FS_VFS
extern const struct luat_vfs_filesystem vfs_fs_posix;
#ifdef LUAT_USE_VFS_INLINE_LIB
extern const char luadb_inline_sys[];
extern const struct luat_vfs_filesystem vfs_fs_luadb;
#endif
#endif


// 文件系统初始化函数
esp_vfs_spiffs_conf_t spiffs_conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 10,
    .format_if_mount_failed = true};

int luat_fs_init(void)
{
  ESP_LOGW(TAG, "Initializing SPIFFS\n");

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
    luat_vfs_reg(&vfs_fs_posix);
	luat_fs_conf_t conf = {
		.busname = "",
		.type = "posix",
		.filesystem = "posix",
		.mount_point = ""
	};
	luat_fs_mount(&conf);
	#ifdef LUAT_USE_VFS_INLINE_LIB
	luat_vfs_reg(&vfs_fs_luadb);
	luat_fs_conf_t conf2 = {
		.busname = (char*)luadb_inline_sys,
		.type = "luadb",
		.filesystem = "luadb",
		.mount_point = "/luadb/",
	};
	luat_fs_mount(&conf2);
	#endif
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
