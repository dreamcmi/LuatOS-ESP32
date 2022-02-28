/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"
#include "luat_rtc.h"
#include "sys/time.h"

int luat_rtc_set(struct tm *tblock)
{
    struct tm t = {0}; 
    t.tm_year = (tblock->tm_year - 1900);
    t.tm_mon = (tblock->tm_mon - 1);
    t.tm_mday = tblock->tm_mday;
    t.tm_hour = tblock->tm_hour;
    t.tm_min = tblock->tm_min;
    t.tm_sec = tblock->tm_sec;
    time_t timeSinceEpoch = mktime(&t);
    struct timeval now = {0};
    now.tv_sec = timeSinceEpoch;
    settimeofday(&now, NULL);
    return 0;
}

int luat_rtc_get(struct tm *tblock)
{
    time_t now = {0};
    char strftime_buf[64] = {0};
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    tblock->tm_sec = timeinfo.tm_sec;
    tblock->tm_min = timeinfo.tm_min;
    tblock->tm_hour = timeinfo.tm_hour;
    tblock->tm_wday = timeinfo.tm_wday;
    tblock->tm_year = (timeinfo.tm_year + 1900);
    tblock->tm_mon = (timeinfo.tm_mon + 1);
    tblock->tm_mday = timeinfo.tm_mday;
    return 0;
}

int luat_rtc_timer_start(int id, struct tm *tblock)
{
    return -1; // 暂不支持
}

int luat_rtc_timer_stop(int id)
{
    return -1; // 暂不支持
}
