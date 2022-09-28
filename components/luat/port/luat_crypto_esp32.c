/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"
#include "luat_crypto.h"

#include "esp_random.h"
int luat_crypto_trng(char *buff, size_t len)
{
    esp_fill_random(buff, len);
    return 0;
}
