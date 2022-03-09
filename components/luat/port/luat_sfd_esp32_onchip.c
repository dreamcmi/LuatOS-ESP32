#include "luat_base.h"
#include "luat_sfd.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_partition.h"

#define LUAT_LOG_TAG "onchip"
#include "luat_log.h"

static esp_partition_t * partition;


int sfd_onchip_init (void* userdata) {
    sfd_onchip_t* onchip = (sfd_onchip_t*)userdata;
    if (onchip == NULL)
       return -1;
    partition = esp_partition_find_first(0x5A, 0x5A, "script");
    if (partition == NULL)
        return -1;;
    onchip->addr = (partition->address - 64 * 1024);
    return 0;
}
int sfd_onchip_status (void* userdata) {
    return 0;
}

int sfd_onchip_read (void* userdata, char* buff, size_t offset, size_t len) {
    sfd_onchip_t* onchip = (sfd_onchip_t*)userdata;
    if (onchip == NULL)
       return -1;
    int ret = spi_flash_read(onchip->addr + offset, buff, len);
    return ret >= 0 ? len : -1;
}

int sfd_onchip_write (void* userdata, const char* buff, size_t offset, size_t len) {
    sfd_onchip_t* onchip = (sfd_onchip_t*)userdata;
    if (onchip == NULL)
       return -1;
    int ret = spi_flash_write(onchip->addr + offset, buff, len);
    return ret >= 0 ? len : -1;
}

int sfd_onchip_erase (void* userdata, size_t offset, size_t len) {
    sfd_onchip_t* onchip = (sfd_onchip_t*)userdata;
    if (onchip == NULL)
       return -1;
    int ret = spi_flash_erase_range(onchip->addr + offset, len);
    return ret;
}

int sfd_onchip_ioctl (void* userdata, size_t cmd, void* buff) {
    return 0;
}
