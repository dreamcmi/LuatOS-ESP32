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


int luat_fs_mkfs(luat_fs_conf_t *conf)
{
}

int luat_fs_mount(luat_fs_conf_t *conf)
{
    esp_vfs_spiffs_register(&conf)
}


int luat_fs_umount(luat_fs_conf_t *conf)
{
    esp_vfs_spiffs_unregister(&conf)
}


int luat_fs_info(const char *path, luat_fs_info_t *conf)
{
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}
