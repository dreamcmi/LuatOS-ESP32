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
      printf("Failed to mount or format filesystem\n");
    }
    else if (ret == ESP_ERR_NOT_FOUND)
    {
      printf("Failed to find SPIFFS partition\n");
    }
    else
    {
      printf("Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret));
    }
    return 1;
  }
  size_t total = 0, used = 0;
  ret = esp_spiffs_info(spiffs_conf.partition_label, &total, &used);
  if (ret != ESP_OK)
  {
    printf("Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
  }
  else
  {
    printf("Partition size: total: %d, used: %d\n", total, used);
  }
  return 0;
}

// int luat_fs_mkfs(luat_fs_conf_t *conf)
// {
//     return 0;
// }

// int luat_fs_mount(luat_fs_conf_t *conf)
// {
//     esp_vfs_spiffs_register(&spiffs_conf);
//     return 0;
// }

// int luat_fs_umount(luat_fs_conf_t *conf)
// {
//     esp_vfs_spiffs_unregister(&spiffs_conf);
//     return 0;
// }

int luat_fs_info(const char *path, luat_fs_info_t *conf)
{
    size_t total = 0, used = 0;
    esp_err_t ret = esp_spiffs_info(spiffs_conf.partition_label, &total, &used);
    if (ret != ESP_OK)
    {
        printf("Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
    }
    else
    {
        printf("Partition size: total: %d, used: %d\n", total, used);
    }
    return 0;
}

