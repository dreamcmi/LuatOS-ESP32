#ifndef _ESPNOW_HANDLE_H
#define _ESPNOW_HANDLE_H

// #define ESPNOW_QUEUE_SIZE 6
// #define ESPNOW_MAXDELAY 512

// typedef struct
// {
//     uint8_t mac_addr[ESP_NOW_ETH_ALEN];
//     esp_now_send_status_t status;
// } espnow_event_send_cb_t;

// typedef struct
// {
//     uint8_t mac_addr[ESP_NOW_ETH_ALEN];
//     uint8_t *data;
//     int data_len;
// } espnow_event_recv_cb_t;

// typedef union
// {
//     espnow_event_send_cb_t *send_cb;
//     espnow_event_recv_cb_t *recv_cb;
// } espnow_event_info_t;

typedef struct
{
    const uint8_t *send_mac_addr;
    int status;

    const uint8_t *recv_mac_addr;
    const uint8_t *data;
    int data_len;
} espnow_event_handle_t;

#endif
