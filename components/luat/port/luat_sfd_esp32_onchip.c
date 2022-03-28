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

static const esp_partition_t * fdb_partition;

int sfd_onchip_init (void* userdata) {
    fdb_partition = esp_partition_find_first(0x5A, 0x5B, "fdb");
    return 0;
}
int sfd_onchip_status (void* userdata) {
    return 0;
}

int sfd_onchip_read (void* userdata, char* buff, size_t offset, size_t len) {
    int ret = esp_partition_read(fdb_partition,  offset, buff, len);
    return ret >= 0 ? len : -1;
}

int sfd_onchip_write (void* userdata, const char* buff, size_t offset, size_t len) {
    int ret = esp_partition_write(fdb_partition, offset, buff, len);
    return ret >= 0 ? len : -1;
}

int sfd_onchip_erase (void* userdata, size_t offset, size_t len) {
    int ret = esp_partition_erase_range(fdb_partition, offset, len);
    return ret >= 0 ? len : -1;
}

int sfd_onchip_ioctl (void* userdata, size_t cmd, void* buff) {
    return 0;
}
