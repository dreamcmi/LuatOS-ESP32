PROJECT = "nimble-gap-broadcast-test"
VERSION = "1.0.0"

sys = require "sys"

function adv_callback(evt)
    if evt.type == nimble.BLE_GAP_EVENT_DISCONNECT then
        log.info(
            "ble",
            "adv_callback",
            "BLE_GAP_EVENT_DISCONNECT",
            evt.disconnect.reason,
            evt.disconnect.conn_handle
        )
        advertise()
    elseif evt.type == nimble.BLE_GAP_EVENT_CONNECT then
        log.info(
            "ble",
            "adv_callback",
            "BLE_GAP_EVENT_CONNECT",
            evt.connect.status,
            evt.connect.conn_handle
        )
    elseif evt.type == nimble.BLE_GAP_EVENT_ADV_COMPLETE then
        log.info("ble", "adv_callback", "BLE_GAP_EVENT_ADV_COMPLETE", evt.adv_complete.reason)
    elseif evt.type == nimble.BLE_GAP_EVENT_CONN_UPDATE then
        log.info("ble", "adv_callback", "BLE_GAP_EVENT_CONN_UPDATE")
    elseif evt.type == nimble.BLE_GAP_EVENT_CONN_UPDATE_REQ then
        log.info("ble", "BLE_GAP_EVENT_CONN_UPDATE_REQ")
    else
        log.error("ble", "adv_callback", "adv_callback", evt.type)
    end
end

function advertise()
    a =
        nimble.gap_adv_set_fields(
        {
            flags = nimble.BLE_HS_ADV_F_DISC_GEN | nimble.BLE_HS_ADV_F_BREDR_UNSUP,
            uuids128 = string.fromHex("00112233445566778899aabbccddeeff"),
            num_uuids128 = 1,
            uuids128_is_complete = 1,
            tx_pwr_lvl = nimble.BLE_HS_ADV_TX_PWR_LVL_AUTO
        }
    )
    log.info("nimble gap_adv_set_fields:", a)
    a =
        nimble.gap_adv_rsp_set_fields(
        {
            name = "nimble_test",
            name_is_complete = 1
        }
    )
    log.info("nimble gap_adv_rsp_set_fields:", a)
    a =
        nimble.gap_adv_start(
        {
            conn_mode = nimble.BLE_GAP_CONN_MODE_UND,
            disc_mode = nimble.BLE_GAP_DISC_MODE_GEN,
            duration_ms = nimble.BLE_HS_FOREVER
            -- duration_ms = 1000
        },
        adv_callback
    )
    log.info("nimble gap_adv_start:", a)
end

function sync_cb()
    log.info("nimble ensure", nimble.hs_util_ensure_addr(0))
    log.info("nimble infer", nimble.hs_id_infer_auto(0))
    advertise()
end

function reset_cb(par)
    log.error("nimble reset:", par)
end

sys.taskInit(
    function()
        -- log.info("nimble set name", nimble.svc_gap_device_name_set("nimble_broad"))
        log.info("nimble run", nimble.run(reset_cb, sync_cb))
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
