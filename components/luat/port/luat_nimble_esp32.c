/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"
#include "luat_msgbus.h"
#include "luat_log.h"
#define LUAT_LOG_TAG "nimble"

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"

static uint8_t g_own_addr_type = 0;

// TODO 简化回调方式
typedef struct luat_uart_cb
{
    int reset_callback; // nimble重置回调
    int sync_callback;  // nimble刷新回调

    int adv_callback;     // gap层广播回调
    int scan_callback;    // gap层扫描回调
    int connect_callback; // gap层连接回调
} luat_nimble_cb_t;

static luat_nimble_cb_t luat_nimble_cb;

static void config_adv_data(lua_State *L, struct ble_hs_adv_fields *f)
{
    if (lua_getfield(L, 1, "flags") == LUA_TNUMBER)
    {
        f->flags = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "uuids16") == LUA_TSTRING)
    {
        size_t len = 0;
        ble_uuid16_t u = {0};
        u.u.type = BLE_UUID_TYPE_16;
        const char *uuid = luaL_checklstring(L, -1, &len);
        if (len == 2)
        {
            memcpy(&u.value, uuid, 2);
            f->uuids16 = &u;
        }
        else
        {
            LLOGE("UUIDS16 len is not fix,input len is %d", len);
        }
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "num_uuids16") == LUA_TNUMBER)
    {
        f->num_uuids16 = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "uuids16_is_complete") == LUA_TNUMBER)
    {
        f->uuids16_is_complete = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "uuids32") == LUA_TSTRING)
    {
        size_t len = 0;
        ble_uuid32_t u = {0};
        u.u.type = BLE_UUID_TYPE_32;
        const char *uuid = luaL_checklstring(L, -1, &len);
        if (len == 4)
        {
            memcpy(&u.value, uuid, 4);
            f->uuids32 = &u;
        }
        else
        {
            LLOGE("UUIDS32 len is not fix,input len is %d", len);
        }
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "num_uuids32") == LUA_TNUMBER)
    {
        f->num_uuids16 = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "uuids32_is_complete") == LUA_TNUMBER)
    {
        f->uuids32_is_complete = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "uuids128") == LUA_TSTRING)
    {
        size_t len = 0;
        ble_uuid128_t u = {0};
        u.u.type = BLE_UUID_TYPE_128;
        const char *uuid = luaL_checklstring(L, -1, &len);
        if (len == 16)
        {
            memcpy(&u.value, uuid, 16);
            f->uuids128 = &u;
        }
        else
        {
            LLOGE("UUIDS128 len is not fix,input len is %d", len);
        }
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "num_uuids128") == LUA_TNUMBER)
    {
        f->num_uuids128 = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "uuids128_is_complete") == LUA_TNUMBER)
    {
        f->uuids128_is_complete = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "name") == LUA_TSTRING)
    {
        const char *n = luaL_checkstring(L, -1);
        f->name = (uint8_t *)n;
        f->name_len = strlen(n);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "name_is_complete") == LUA_TNUMBER)
    {
        f->name_is_complete = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "tx_pwr_lvl") == LUA_TNUMBER)
    {
        f->tx_pwr_lvl = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "tx_pwr_lvl_is_present") == LUA_TNUMBER)
    {
        f->tx_pwr_lvl_is_present = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "slave_itvl_range") == LUA_TSTRING)
    {
        size_t len = 0;
        f->slave_itvl_range = (const uint8_t *)luaL_checklstring(L, -1, &len);
        // const char *m = luaL_checklstring(L, -1, &len);
        // memcpy(f->slave_itvl_range, m, len);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "svc_data_uuid16") == LUA_TSTRING)
    {
        size_t len = 0;
        f->svc_data_uuid16 = (const uint8_t *)luaL_checklstring(L, -1, &len);
        // const char *m = luaL_checklstring(L, -1, &len);
        // memcpy(f->svc_data_uuid16, m, len);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "svc_data_uuid16_len") == LUA_TNUMBER)
    {
        f->svc_data_uuid16_len = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "public_tgt_addr") == LUA_TSTRING)
    {
        size_t len = 0;
        f->public_tgt_addr = (const uint8_t *)luaL_checklstring(L, -1, &len);
        // const char *m = luaL_checklstring(L, -1, &len);
        // memcpy(f->public_tgt_addr, m, len);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "num_public_tgt_addrs") == LUA_TNUMBER)
    {
        f->num_public_tgt_addrs = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "appearance") == LUA_TNUMBER)
    {
        f->appearance = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "appearance_is_present") == LUA_TNUMBER)
    {
        f->appearance_is_present = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "adv_itvl") == LUA_TNUMBER)
    {
        f->adv_itvl = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "adv_itvl_is_present") == LUA_TNUMBER)
    {
        f->adv_itvl_is_present = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "svc_data_uuid16") == LUA_TSTRING)
    {
        size_t len = 0;
        f->svc_data_uuid16 = (const uint8_t *)luaL_checklstring(L, -1, &len);
        // const char *m = luaL_checklstring(L, -1, &len);
        // memcpy(f->svc_data_uuid16, m, len);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "svc_data_uuid16_len") == LUA_TNUMBER)
    {
        f->svc_data_uuid16_len = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "svc_data_uuid32") == LUA_TSTRING)
    {
        size_t len = 0;
        f->svc_data_uuid32 = (const uint8_t *)luaL_checklstring(L, -1, &len);
        // const char *m = luaL_checklstring(L, -1, &len);
        // memcpy(f->svc_data_uuid32, m, len);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "svc_data_uuid32_len") == LUA_TNUMBER)
    {
        f->svc_data_uuid32_len = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "svc_data_uuid128") == LUA_TSTRING)
    {
        size_t len = 0;
        f->svc_data_uuid128 = (const uint8_t *)luaL_checklstring(L, -1, &len);
        // const char *m = luaL_checklstring(L, -1, &len);
        // memcpy(f->svc_data_uuid128, m, len);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "svc_data_uuid128_len") == LUA_TNUMBER)
    {
        f->svc_data_uuid128_len = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "uri") == LUA_TSTRING)
    {
        size_t len = 0;
        f->uri = (const uint8_t *)luaL_checklstring(L, -1, &len);
        // const char *m = luaL_checklstring(L, -1, &len);
        // memcpy(f->uri, m, len);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "uri_len") == LUA_TNUMBER)
    {
        f->uri_len = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "mfg_data") == LUA_TSTRING)
    {
        size_t len = 0;
        f->mfg_data = (const uint8_t *)luaL_checklstring(L, -1, &len);
        // const char *m = luaL_checklstring(L, -1, &len);
        // memcpy(f->mfg_data, m, len);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "mfg_data_len") == LUA_TNUMBER)
    {
        f->mfg_data_len = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);
}

/////////////////connect event begin/////////////////////////
static int luat_nimble_connect_event(lua_State *L, void *ptr)
{
    struct ble_gap_event *event = (struct ble_gap_event *)ptr;
    if (!luat_nimble_cb.adv_callback)
    {
        LLOGE("luat_nimble_reset_event get cb error");
        return 0;
    }
    lua_geti(L, LUA_REGISTRYINDEX, luat_nimble_cb.adv_callback);

    lua_newtable(L);
    lua_pushliteral(L, "type");
    lua_pushinteger(L, event->type); // t["type"]
    lua_settable(L, -3);

    lua_pushliteral(L, "connect");
    lua_newtable(L);             // t["connect"] = {}
    lua_pushliteral(L, "status"); // t["connect"]["status"]
    lua_pushinteger(L, event->connect.status);
    lua_settable(L, -3);
    lua_pushliteral(L, "conn_handle"); // t["connect"]["conn_handle"]
    lua_pushinteger(L, event->connect.conn_handle);
    lua_settable(L, -3);
    lua_settable(L, -3);

    lua_pushliteral(L, "disconnect");
    lua_newtable(L);             // t["disconnect"] = {}
    lua_pushliteral(L, "reason"); // t["disconnect"]["reason"]
    lua_pushinteger(L, event->disconnect.reason);
    lua_settable(L, -3);
    lua_pushliteral(L, "conn_handle"); // t["disconnect"]["conn_handle"]
    lua_pushinteger(L, event->disconnect.conn.conn_handle);
    lua_settable(L, -3);
    lua_settable(L, -3);
    lua_call(L, 1, 0);

    free(event);
    return 0;
}

static int gap_connect_event(struct ble_gap_event *event, void *arg)
{
    // printf("event:%d\n", event->type);
    rtos_msg_t msg = {0};
    struct ble_gap_event *ev = malloc(sizeof(struct ble_gap_event));
    memcpy(ev, event, sizeof(struct ble_gap_event));

    msg.handler = luat_nimble_connect_event;
    msg.ptr = ev;
    msg.arg1 = 0;
    msg.arg2 = 0;
    luat_msgbus_put(&msg, 0);
    return 0;
}
/////////////////connect event end/////////////////////////

/////////////////scan event begin/////////////////////////
static int luat_nimble_scan_event(lua_State *L, void *ptr)
{
    struct ble_gap_event *event = (struct ble_gap_event *)ptr;
    if (!luat_nimble_cb.adv_callback)
    {
        LLOGE("luat_nimble_reset_event get cb error");
        return 0;
    }
    lua_geti(L, LUA_REGISTRYINDEX, luat_nimble_cb.adv_callback);

    lua_newtable(L);
    lua_pushliteral(L, "type");
    lua_pushinteger(L, event->type); // t["type"]
    lua_settable(L, -3);

    lua_pushliteral(L, "disc");
    lua_newtable(L);                 // t["disc"] = {}
    lua_pushliteral(L, "event_type"); // t["disc"]["event_type"]
    lua_pushinteger(L, event->disc.event_type);
    lua_settable(L, -3);
    lua_pushliteral(L, "rssi"); // t["disc"]["rssi"]
    lua_pushinteger(L, event->disc.rssi);
    lua_settable(L, -3);
    lua_pushliteral(L, "addr"); // t["disc"]["addr"]
    lua_pushlstring(L, (const char *)event->disc.addr.val, 6);
    lua_settable(L, -3);
    lua_pushliteral(L, "addr_type"); // t["disc"]["addr_type"]
    lua_pushinteger(L, event->disc.addr.type);
    lua_settable(L, -3);
    lua_pushliteral(L, "data"); // t["disc"]["data"]
    lua_pushlstring(L, (const char *)event->disc.data, event->disc.length_data);
    lua_settable(L, -3);
    lua_settable(L, -3);

    lua_pushliteral(L, "disc_complete");
    lua_newtable(L);             // t["disc_complete"] = {}
    lua_pushliteral(L, "reason"); // t["disc_complete"]["reason"]
    lua_pushinteger(L, event->disc_complete.reason);
    lua_settable(L, -3);
    lua_settable(L, -3);

    lua_call(L, 1, 0);

    free(event);
    return 0;
}

static int gap_scan_event(struct ble_gap_event *event, void *arg)
{
    // printf("event:%d\n", event->type);
    rtos_msg_t msg = {0};
    struct ble_gap_event *ev = malloc(sizeof(struct ble_gap_event));
    memcpy(ev, event, sizeof(struct ble_gap_event));

    msg.handler = luat_nimble_scan_event;
    msg.ptr = ev;
    msg.arg1 = 0;
    msg.arg2 = 0;
    luat_msgbus_put(&msg, 0);
    return 0;
}
/////////////////scan event end/////////////////////////

/////////////////adv event begin/////////////////////////
static int luat_nimble_adv_event(lua_State *L, void *ptr)
{
    struct ble_gap_event *event = (struct ble_gap_event *)ptr;
    if (!luat_nimble_cb.adv_callback)
    {
        LLOGE("luat_nimble_reset_event get cb error");
        return 0;
    }
    lua_geti(L, LUA_REGISTRYINDEX, luat_nimble_cb.adv_callback);

    lua_newtable(L);
    lua_pushliteral(L, "type");
    lua_pushinteger(L, event->type); // t["type"]
    lua_settable(L, -3);

    lua_pushliteral(L, "connect");
    lua_newtable(L);             // t["connect"] = {}
    lua_pushliteral(L, "status"); // t["connect"]["status"]
    lua_pushinteger(L, event->connect.status);
    lua_settable(L, -3);
    lua_pushliteral(L, "conn_handle"); // t["connect"]["conn_handle"]
    lua_pushinteger(L, event->connect.conn_handle);
    lua_settable(L, -3);
    lua_settable(L, -3);

    lua_pushliteral(L, "disconnect");
    lua_newtable(L);             // t["disconnect"] = {}
    lua_pushliteral(L, "reason"); // t["disconnect"]["reason"]
    lua_pushinteger(L, event->disconnect.reason);
    lua_settable(L, -3);
    lua_pushliteral(L, "conn_handle"); // t["disconnect"]["conn_handle"]
    lua_pushinteger(L, event->disconnect.conn.conn_handle);
    lua_settable(L, -3);
    lua_settable(L, -3);

    lua_pushliteral(L, "adv_complete"); // t["adv_complete"] = {}
    lua_newtable(L);
    lua_pushliteral(L, "reason"); // t["adv_complete"]["reason"]
    lua_pushinteger(L, event->adv_complete.reason);
    lua_settable(L, -3);
    lua_settable(L, -3);

    lua_call(L, 1, 0);

    free(event);
    return 0;
}

static int gap_adv_event(struct ble_gap_event *event, void *arg)
{
    // printf("event:%d\n", event->type);
    rtos_msg_t msg = {0};
    struct ble_gap_event *ev = malloc(sizeof(struct ble_gap_event));
    memcpy(ev, event, sizeof(struct ble_gap_event));

    msg.handler = luat_nimble_adv_event;
    msg.ptr = ev;
    msg.arg1 = 0;
    msg.arg2 = 0;
    luat_msgbus_put(&msg, 0);
    return 0;
}
/////////////////adv event end/////////////////////////

/////////////////reset event begin/////////////////////////
static int luat_nimble_reset_event(lua_State *L, void *ptr)
{
    rtos_msg_t *msg = (rtos_msg_t *)lua_topointer(L, -1);
    if (luat_nimble_cb.reset_callback)
    {
        lua_geti(L, LUA_REGISTRYINDEX, luat_nimble_cb.reset_callback);
        lua_pushinteger(L, msg->arg1);
        lua_call(L, 1, 0);
    }
    else
    {
        MODLOG_DFLT(ERROR, "luat_nimble_reset_event get cb error");
    }
    return 0;
}

static void bleprph_on_reset(int reason)
{
    rtos_msg_t msg = {0};
    msg.handler = luat_nimble_reset_event;
    msg.ptr = NULL;
    msg.arg1 = reason;
    msg.arg2 = 0;
    luat_msgbus_put(&msg, 0);
    // MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}
/////////////////reset event end/////////////////////////

/////////////////sync event begin/////////////////////////
static int luat_nimble_sync_event(lua_State *L, void *ptr)
{
    if (luat_nimble_cb.sync_callback)
    {
        lua_geti(L, LUA_REGISTRYINDEX, luat_nimble_cb.sync_callback);
        lua_call(L, 0, 0);
    }
    else
    {
        MODLOG_DFLT(ERROR, "luat_nimble_sync_event get cb error");
    }
    return 0;
}

static void bleprph_on_sync(void)
{
    rtos_msg_t msg = {0};
    msg.handler = luat_nimble_sync_event;
    msg.ptr = NULL;
    msg.arg1 = 1;
    msg.arg2 = 0;
    luat_msgbus_put(&msg, 0);
}
/////////////////sync event end/////////////////////////

// esp-idf nimble主进程
void bleprph_host_task(void *param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}

/*
nimble.run(reset_cb,sync_cb)
*/
static int l_nimble_run(lua_State *L)
{
    if (lua_isfunction(L, 1))
    {
        lua_pushvalue(L, 1);
        luat_nimble_cb.reset_callback = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        lua_pushboolean(L, -1);
        return 1;
    }
    if (lua_isfunction(L, 2))
    {
        lua_pushvalue(L, 2);
        luat_nimble_cb.sync_callback = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        lua_pushboolean(L, -2);
        return 1;
    }

    esp_nimble_hci_and_controller_init();
    nimble_port_init();
    ble_hs_cfg.reset_cb = bleprph_on_reset;
    ble_hs_cfg.sync_cb = bleprph_on_sync;
    nimble_port_freertos_init(bleprph_host_task);
    lua_pushboolean(L, 0);
    return 1;
}

/*
nimble.stop()
*/
static int l_nimble_stop(lua_State *L)
{
    lua_pushinteger(L, nimble_port_stop());
    return 1;
}

/*
nimble.svc_gap_device_name_set("nimble_prph")
*/
static int l_nimble_svc_gap_device_name_set(lua_State *L)
{
    lua_pushinteger(L, ble_svc_gap_device_name_set(luaL_checkstring(L, 1)));
    return 1;
}

/*
nimble.hs_util_ensure_addr(0)
*/
static int l_nimble_hs_util_ensure_addr(lua_State *L)
{
    lua_pushinteger(L, ble_hs_util_ensure_addr(luaL_optinteger(L, 1, 0)));
    return 1;
}

/*
nimble.hs_id_infer_auto(0)
*/
static int l_nimble_hs_id_infer_auto(lua_State *L)
{
    lua_pushinteger(L, ble_hs_id_infer_auto(luaL_optinteger(L, 1, 0), &g_own_addr_type));
    return 1;
}

/*
err,addr_table = nimble.hs_id_gen_rnd(0)
*/
static int l_nimble_hs_id_gen_rnd(lua_State *L)
{
    ble_addr_t addr = {0};
    int mode = luaL_checkinteger(L, 1);
    lua_pushinteger(L, ble_hs_id_gen_rnd(mode, &addr));

    lua_newtable(L);
    lua_pushstring(L, "val");
    lua_pushlstring(L, (const char *)addr.val, 6);
    lua_settable(L, -3);
    lua_pushstring(L, "type");
    lua_pushinteger(L, addr.type);
    lua_settable(L, -3);

    return 2;
}

/*
-- 需要nimble.hs_id_gen_rnd返回的table
nimble.hs_id_set_rnd(addr_table.val)
*/
static int l_nimble_hs_id_set_rnd(lua_State *L)
{
    size_t len = 0;
    const uint8_t *val = (const uint8_t *)luaL_checklstring(L, 1, &len);
    lua_pushinteger(L, ble_hs_id_set_rnd(val));
    return 1;
}

/*
nimble.gap_adv_set_fields({
    flags = nimble.BLE_HS_ADV_F_DISC_GEN | nimble.BLE_HS_ADV_F_BREDR_UNSUP,
    uuids128 = string.fromHex("00112233445566778899aabbccddeeff"),
    num_uuids128 = 1,
    uuids128_is_complete = 1,
    tx_pwr_lvl = nimble.BLE_HS_ADV_TX_PWR_LVL_AUTO
})
*/
static int l_nimble_gap_adv_set_fields(lua_State *L)
{
    if (!lua_istable(L, 1))
    {
        lua_pushstring(L, "l_nimble_gap_adv_set_fields need table");
        lua_error(L);
        return 0;
    }

    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));

    config_adv_data(L, &fields);

    lua_pushinteger(L, ble_gap_adv_set_fields(&fields));
    return 1;
}

/*
nimble.gap_adv_set_data(string.fromHex("aabbccdd"),4)
*/
static int l_nimble_gap_adv_set_data(lua_State *L)
{
    uint8_t *data = NULL;
    size_t len = 0;
    const char *n = luaL_checklstring(L, 1, &len);
    int data_len = luaL_checkinteger(L, 2);
    if (data_len <= len)
    {
        memcpy(data, n, data_len);
    }
    else
    {
        memcpy(data, n, len);
    }

    lua_pushinteger(L, ble_gap_adv_set_data(data, data_len));
    return 1;
}

/*
nimble.gap_adv_rsp_set_fields({
    name = "nimble_test",
    name_is_complete = 1
})
*/
static int l_nimble_gap_adv_rsp_set_fields(lua_State *L)
{
    if (!lua_istable(L, 1))
    {
        lua_pushstring(L, "l_nimble_gap_adv_set_fields need table");
        lua_error(L);
        return 0;
    }

    struct ble_hs_adv_fields rsp_fields;
    memset(&rsp_fields, 0, sizeof(rsp_fields));

    config_adv_data(L, &rsp_fields);

    lua_pushinteger(L, ble_gap_adv_rsp_set_fields(&rsp_fields));
    return 1;
}

/*
nimble.gap_adv_rsp_set_data(string.fromHex("aabbccdd"),4)
*/
static int l_nimble_gap_adv_rsp_set_data(lua_State *L)
{
    uint8_t *data = NULL;
    size_t len = 0;
    const char *n = luaL_checklstring(L, 1, &len);
    int data_len = luaL_checkinteger(L, 2);
    if (data_len <= len)
    {
        memcpy(data, n, data_len);
    }
    else
    {
        memcpy(data, n, len);
    }

    lua_pushinteger(L, ble_gap_adv_rsp_set_data(data, data_len));
    return 1;
}

/*
nimble.gap_adv_start({
    conn_mode = nimble.BLE_GAP_CONN_MODE_UND,
    disc_mode = nimble.BLE_GAP_DISC_MODE_GEN,
    duration_ms = nimble.BLE_HS_FOREVER
},adv_callback)
*/
static int l_nimble_gap_adv_start(lua_State *L)
{
    if (!lua_istable(L, 1))
    {
        lua_pushinteger(L, -1);
        return 1;
    }

    if (lua_isfunction(L, 2))
    {
        lua_pushvalue(L, 2);
        luat_nimble_cb.adv_callback = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        lua_pushinteger(L, -2);
        return 1;
    }

    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    int32_t duration_ms = 0xffff;

    if (lua_getfield(L, 1, "conn_mode") == LUA_TNUMBER)
    {
        adv_params.conn_mode = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "disc_mode") == LUA_TNUMBER)
    {
        adv_params.disc_mode = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "itvl_min") == LUA_TNUMBER)
    {
        adv_params.itvl_min = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "itvl_max") == LUA_TNUMBER)
    {
        adv_params.itvl_max = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "channel_map") == LUA_TNUMBER)
    {
        adv_params.channel_map = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "filter_policy") == LUA_TNUMBER)
    {
        adv_params.filter_policy = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "high_duty_cycle") == LUA_TNUMBER)
    {
        adv_params.high_duty_cycle = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "duration_ms") == LUA_TNUMBER)
    {
        duration_ms = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    lua_pushinteger(L, ble_gap_adv_start(g_own_addr_type, NULL, duration_ms, &adv_params, gap_adv_event, NULL));
    return 1;
}

/*
nimble.gap_adv_stop()
*/
static int l_nimble_gap_adv_stop(lua_State *L)
{
    lua_pushinteger(L, ble_gap_adv_stop());
    return 1;
}

/*
nimble.gap_disc({
    itvl = 10000,
    window = 200,
    filter_policy = 0,
    limited = 0,
    passive = 0,
    filter_duplicates = 1
},disc_cb)
*/
static int l_nimble_gap_disc(lua_State *L)
{
    if (!lua_istable(L, 1))
    {
        lua_pushinteger(L, -1);
        return 1;
    }

    if (lua_isfunction(L, 2))
    {
        lua_pushvalue(L, 2);
        luat_nimble_cb.scan_callback = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        lua_pushinteger(L, -1);
        return 1;
    }

    struct ble_gap_disc_params disc_params;
    memset(&disc_params, 0, sizeof(disc_params));

    if (lua_getfield(L, 1, "itvl") == LUA_TNUMBER)
    {
        disc_params.itvl = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "window") == LUA_TNUMBER)
    {
        disc_params.window = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "filter_policy") == LUA_TNUMBER)
    {
        disc_params.filter_policy = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "limited") == LUA_TNUMBER)
    {
        disc_params.limited = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "passive") == LUA_TNUMBER)
    {
        disc_params.passive = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 1, "filter_duplicates") == LUA_TNUMBER)
    {
        disc_params.filter_duplicates = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    lua_pushinteger(L, ble_gap_disc(g_own_addr_type, BLE_HS_FOREVER, &disc_params, gap_scan_event, NULL));
    return 1;
}

/*
nimble.gap_disc_cancel()
*/
static int l_nimble_gap_disc_cancel(lua_State *L)
{
    lua_pushinteger(L, ble_gap_disc_cancel());
    return 1;
}

/*
-- 要在gap_cb里调用
nimble.hs_adv_parse_fields(
    event.data,
    #event.data)
*/
static int l_nimble_hs_adv_parse_fields(lua_State *L)
{
    size_t len = 0;
    uint8_t *data = NULL;
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));

    const char *d = luaL_checklstring(L, 1, &len);
    memcpy(data, d, len);

    lua_pushinteger(L, ble_hs_adv_parse_fields(&fields, data, len));
    // TODO 返回fields的table类型
    return 2;
}

/*
nimble.gap_connect(addr,time,connect_cb)
*/
static int l_nimble_gap_connect(lua_State *L)
{
    size_t len = 0;
    ble_addr_t peer_addr = {0};
    uint8_t *addr = (uint8_t *)luaL_checklstring(L, 1, &len);
    peer_addr.type = luaL_checkinteger(L, 2);
    int32_t duration_ms = luaL_checkinteger(L, 3);
    if (len == 6)
    {
        memcpy(peer_addr.val, addr, 6);
    }
    else
    {
        LLOGE("addr len is not fix,input len is %d", len);
        lua_pushinteger(L, -1);
        return 1;
    }

    if (lua_isfunction(L, 3))
    {
        lua_pushvalue(L, 3);
        luat_nimble_cb.connect_callback = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        lua_pushinteger(L, -1);
        return 1;
    }

    lua_pushinteger(L, ble_gap_connect(g_own_addr_type, &peer_addr, duration_ms, NULL, gap_connect_event, NULL));
    return 1;
}

/*
nimble.gap_terminate(
    event.connect.conn_handle,
    0x13)
*/
static int l_nimble_gap_terminate(lua_State *L)
{
    uint16_t conn_handle = luaL_checkinteger(L, 1);
    uint8_t reason = luaL_checkinteger(L, 2);
    lua_pushinteger(L, ble_gap_terminate(conn_handle, reason));
    return 1;
}

/*
nimble.ibeacon_set_adv_data(
    string.fromHex("00112233445566778899aabbccddeeff"),
    0x1234,
    0x5678,
    10)
*/
static int l_nimble_ibeacon_set_adv_data(lua_State *L)
{
    uint8_t *uuid128 = NULL;
    size_t len = 0;
    const char *n = luaL_checklstring(L, 1, &len);
    memcpy(uuid128, n, len);
    uint16_t major = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t minor = (uint16_t)luaL_checkinteger(L, 3);
    int8_t measured_power = (int8_t)luaL_checkinteger(L, 4);

    lua_pushinteger(L, ble_ibeacon_set_adv_data(uuid128, major, minor, measured_power));
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_espnimble[] =
    {
        {"run", l_nimble_run, 0},   // 运行
        {"stop", l_nimble_stop, 0}, // 停止

        {"svc_gap_device_name_set", l_nimble_svc_gap_device_name_set, 0}, // 设置设备名称
        {"hs_id_gen_rnd", l_nimble_hs_id_gen_rnd, 0},                     // 生成一个新的随机地址
        {"hs_id_set_rnd", l_nimble_hs_id_set_rnd, 0},                     // 设置设备随机地址

        {"hs_util_ensure_addr", l_nimble_hs_util_ensure_addr, 0}, // 确认mac类型
        {"hs_id_infer_auto", l_nimble_hs_id_infer_auto, 0},       // 自动配置mac

        {"gap_adv_set_fields", l_nimble_gap_adv_set_fields, 0}, // 设置gap广播数据
        {"gap_adv_set_data", l_nimble_gap_adv_set_data, 0},     // 设置gap广播数据,raw格式

        {"ibeacon_set_adv_data", l_nimble_ibeacon_set_adv_data, 0}, // ibeacon广播包设置

        {"gap_adv_rsp_set_fields", l_nimble_gap_adv_rsp_set_fields, 0}, // 设备gap广播回复数据
        {"gap_adv_rsp_set_data", l_nimble_gap_adv_rsp_set_data, 0},     // 设备gap广播回复数据,raw格式

        {"gap_adv_start", l_nimble_gap_adv_start, 0}, // 开始gap广播
        {"gap_adv_stop", l_nimble_gap_adv_stop, 0},   // 停止gap广播

        {"gap_disc", l_nimble_gap_disc, 0},               // 开始gap扫描
        {"gap_disc_cancel", l_nimble_gap_disc_cancel, 0}, // 取消gap扫描

        {"hs_adv_parse_fields", l_nimble_hs_adv_parse_fields, 0}, // 解析gap数据

        {"gap_connect", l_nimble_gap_connect, 0},     // gap连接设备
        {"gap_terminate", l_nimble_gap_terminate, 0}, // gap断开设备

        {"BLE_GAP_CONN_MODE_UND", NULL, BLE_GAP_CONN_MODE_UND},
        {"BLE_GAP_CONN_MODE_DIR", NULL, BLE_GAP_CONN_MODE_DIR},
        {"BLE_GAP_CONN_MODE_NON", NULL, BLE_GAP_CONN_MODE_NON},

        {"BLE_GAP_DISC_MODE_GEN", NULL, BLE_GAP_DISC_MODE_GEN},
        {"BLE_GAP_DISC_MODE_LTD", NULL, BLE_GAP_DISC_MODE_LTD},
        {"BLE_GAP_DISC_MODE_NON", NULL, BLE_GAP_DISC_MODE_NON},

        {"BLE_HS_ADV_F_DISC_LTD", NULL, BLE_HS_ADV_F_DISC_LTD},
        {"BLE_HS_ADV_F_DISC_GEN", NULL, BLE_HS_ADV_F_DISC_GEN},
        {"BLE_HS_ADV_F_BREDR_UNSUP", NULL, BLE_HS_ADV_F_BREDR_UNSUP},

        {"BLE_HS_FOREVER", NULL, BLE_HS_FOREVER},

        {"BLE_HS_ADV_TX_PWR_LVL_AUTO", NULL, BLE_HS_ADV_TX_PWR_LVL_AUTO},

        {"BLE_GAP_EVENT_CONNECT", NULL, BLE_GAP_EVENT_CONNECT},
        {"BLE_GAP_EVENT_DISCONNECT", NULL, BLE_GAP_EVENT_DISCONNECT},
        {"BLE_GAP_EVENT_CONN_UPDATE", NULL, BLE_GAP_EVENT_CONN_UPDATE},
        {"BLE_GAP_EVENT_CONN_UPDATE_REQ", NULL, BLE_GAP_EVENT_CONN_UPDATE_REQ},
        {"BLE_GAP_EVENT_DISC", NULL, BLE_GAP_EVENT_DISC},
        {"BLE_GAP_EVENT_DISC_COMPLETE", NULL, BLE_GAP_EVENT_DISC_COMPLETE},
        {"BLE_GAP_EVENT_ADV_COMPLETE", NULL, BLE_GAP_EVENT_ADV_COMPLETE},
        {"BLE_GAP_EVENT_MTU", NULL, BLE_GAP_EVENT_MTU},

        {NULL, NULL, 0}};

LUAMOD_API int luaopen_espnimble(lua_State *L)
{
    luat_newlib(L, reg_espnimble);
    return 1;
}
