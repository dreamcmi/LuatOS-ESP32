/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  ntp
@summary esp32专用ntp库
@version 1.0
@date    2022.2.15
*/
#include "luat_base.h"
#include "luat_msgbus.h"

#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sntp.h"

static int l_ntp_handler(lua_State *L, void *ptr)
{
    rtos_msg_t *msg = (rtos_msg_t *)lua_topointer(L, -1);
    if (msg->arg1 == 1)
    {
        lua_getglobal(L, "sys_pub");
/*
@sys_pub ntp
ntp更新完成
NTP_SYNC_DONE
@usage
sys.subscribe("NTP_SYNC_DONE", function ()
    log.info("ntp", "NTP_SYNC_DONE")
end)
*/
        lua_pushstring(L, "NTP_SYNC_DONE");
        lua_call(L, 1, 0);
    }
    return 0;
}

void time_sync_notification_cb(struct timeval *tv)
{
    // ESP_LOGI("lntp", "Notification of a time synchronization event");
    rtos_msg_t msg;
    msg.handler = l_ntp_handler;
    msg.ptr = NULL;
    msg.arg1 = 1;
    msg.arg2 = 0;
    luat_msgbus_put(&msg, 1);
}

/*
ntp初始化
@api ntp.init(servername)
@string ntp服务器地址
@return 0
@usage
ntp.init("ntp.ntsc.ac.cn")
*/
static int l_ntp_init(lua_State *L)
{
    size_t len = 0;
    const char *sname = luaL_checklstring(L, 1, &len);
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, sname);
    sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
    return 0;
}

/*
ntp设置时区
@api ntp.settz(tz)
#string 时区 具体参数请看(https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html)
@return 0
@usage
ntp.settz("CST-8")
*/
static int l_ntp_settz(lua_State *L)
{
    size_t len = 0;
    const char *utz = luaL_checklstring(L, 1, &len);
    setenv("TZ", utz, 1);
    tzset();
    return 0;
}

#include "rotable.h"
static const rotable_Reg reg_ntp[] =
    {
        {"init", l_ntp_init, 0},
        {"settz", l_ntp_settz, 0},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_ntp(lua_State *L)
{
    luat_newlib(L, reg_ntp);
    return 1;
}
