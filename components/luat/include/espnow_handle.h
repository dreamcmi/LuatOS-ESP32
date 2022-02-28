/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESPNOW_HANDLE_H
#define _ESPNOW_HANDLE_H


typedef struct
{
    const uint8_t *send_mac_addr;
    int status;

    const uint8_t *recv_mac_addr;
    const uint8_t *data;
    int data_len;
} espnow_event_handle_t;

#endif
