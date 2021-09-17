// #include "luat_base.h"
// #include "luat_ble_esp32.h"
// #include "luat_log.h"
// #define LUAT_LOG_TAG "luat.ble"

// #include "esp_bt.h"
// #include "esp_gap_ble_api.h"
// #include "esp_gattc_api.h"
// #include "esp_gatt_defs.h"
// #include "esp_bt_main.h"
// #include "esp_bt_defs.h"
// #include "string.h"

// const uint8_t uuid_zeros[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// esp_ble_adv_params_t ble_adv_params = {0};
// // esp_ble_ibeacon_head_t ibeacon_common_head = {0};
// esp_ble_ibeacon_vendor_t vendor_config = {0};

// esp_ble_ibeacon_head_t ibeacon_common_head = {
//     .flags = {0x02, 0x01, 0x06},
//     .length = 0x1A,
//     .type = 0xFF,
//     .company_id = 0x004C,
//     .beacon_type = 0x1502};

// bool esp_ble_is_ibeacon_packet(uint8_t *adv_data, uint8_t adv_data_len)
// {
//     bool result = false;
//     if ((adv_data != NULL) && (adv_data_len == 0x1E))
//     {
//         if (!memcmp(adv_data, (uint8_t *)&ibeacon_common_head, sizeof(ibeacon_common_head)))
//         {
//             result = true;
//         }
//     }
//     return result;
// }

// static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
// {
//     esp_err_t err;

//     switch (event)
//     {
//     case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
//     {
//         esp_ble_gap_start_advertising(&ble_adv_params);
//         break;
//     }
//     case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
//     {
//         //the unit of the duration is second, 0 means scan permanently
//         uint32_t duration = 0;
//         esp_ble_gap_start_scanning(duration);
//         break;
//     }
//     case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
//         //scan start complete event to indicate scan start successfully or failed
//         if ((err = param->scan_start_cmpl.status) != ESP_BT_STATUS_SUCCESS)
//         {
//             LLOGE("Scan start failed: %s", esp_err_to_name(err));
//         }
//         break;
//     case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
//         //adv start complete event to indicate adv start successfully or failed
//         if ((err = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS)
//         {
//             LLOGE("Adv start failed: %s", esp_err_to_name(err));
//         }
//         break;
//     case ESP_GAP_BLE_SCAN_RESULT_EVT:
//     {
//         esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
//         switch (scan_result->scan_rst.search_evt)
//         {
//         case ESP_GAP_SEARCH_INQ_RES_EVT:
//             /* Search for BLE iBeacon Packet */
//             if (esp_ble_is_ibeacon_packet(scan_result->scan_rst.ble_adv, scan_result->scan_rst.adv_data_len))
//             {
//                 esp_ble_ibeacon_t *ibeacon_data = (esp_ble_ibeacon_t *)(scan_result->scan_rst.ble_adv);
//                 LLOGI("----------iBeacon Found----------");
//                 LLOGI("IBEACON_DEMO: Device address:", scan_result->scan_rst.bda, ESP_BD_ADDR_LEN);
//                 LLOGI("IBEACON_DEMO: Proximity UUID:", ibeacon_data->ibeacon_vendor.proximity_uuid, ESP_UUID_LEN_128);

//                 uint16_t major = ENDIAN_CHANGE_U16(ibeacon_data->ibeacon_vendor.major);
//                 uint16_t minor = ENDIAN_CHANGE_U16(ibeacon_data->ibeacon_vendor.minor);
//                 LLOGI("Major: 0x%04x (%d)", major, major);
//                 LLOGI("Minor: 0x%04x (%d)", minor, minor);
//                 LLOGI("Measured power (RSSI at a 1m distance):%d dbm", ibeacon_data->ibeacon_vendor.measured_power);
//                 LLOGI("RSSI of packet:%d dbm", scan_result->scan_rst.rssi);
//             }
//             break;
//         default:
//             break;
//         }
//         break;
//     }
//     case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
//         if ((err = param->scan_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS)
//         {
//             LLOGE("Scan stop failed: %s", esp_err_to_name(err));
//         }
//         else
//         {
//             LLOGI("Stop scan successfully");
//         }
//         break;

//     case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
//         if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS)
//         {
//             LLOGE("Adv stop failed: %s", esp_err_to_name(err));
//         }
//         else
//         {
//             LLOGI("Stop adv successfully");
//         }
//         break;
//     default:
//         break;
//     }
// }

// esp_err_t esp_ble_config_ibeacon_data(esp_ble_ibeacon_vendor_t *vendor_config, esp_ble_ibeacon_t *ibeacon_adv_data)
// {
//     if ((vendor_config == NULL) || (ibeacon_adv_data == NULL) || (!memcmp(vendor_config->proximity_uuid, uuid_zeros, sizeof(uuid_zeros))))
//     {
//         return ESP_ERR_INVALID_ARG;
//     }
//     memcpy(&ibeacon_adv_data->ibeacon_head, &ibeacon_common_head, sizeof(esp_ble_ibeacon_head_t));
//     memcpy(&ibeacon_adv_data->ibeacon_vendor, vendor_config, sizeof(esp_ble_ibeacon_vendor_t));
//     return ESP_OK;
// }

// int l_ble_adv_init(lua_State *L)
// {
//     ble_adv_params.adv_int_min = (uint16_t)luaL_checknumber(L, 1);
//     ble_adv_params.adv_int_max = (uint16_t)luaL_checknumber(L, 2);
//     ble_adv_params.adv_type = (uint8_t)luaL_checknumber(L, 3);
//     ble_adv_params.own_addr_type = (uint8_t)luaL_checknumber(L, 4);
//     //TODO:
//     //peer_addr5  peer_addr_type6
//     ble_adv_params.channel_map = (uint8_t)luaL_checknumber(L, 7);
//     ble_adv_params.adv_filter_policy = (uint8_t)luaL_checknumber(L, 8);

//     esp_bluedroid_init();
//     esp_bluedroid_enable();
//     esp_err_t status;
//     if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK)
//     {
//         LLOGE("gap register error: %s", esp_err_to_name(status));
//         lua_pushboolean(L, 0);
//         return -1;
//     }
//     return 0;
//     lua_pushboolean(L, 1);
// }

// int l_ble_ibeacon_init(lua_State *L)
// {
//     /*暂不可用 TODO*/
//     esp_ble_ibeacon_t ibeacon_adv_data;
//     size_t len = 16;
//     const char *uuid = luaL_checklstring(L, 1, &len);
//     // vendor_config.proximity_uuid = {uuidd[0], uuidd[1], uuidd[2], uuidd[3], uuidd[4], uuidd[5], uuidd[6], uuidd[7], uuidd[8], uuidd[9], uuidd[10], uuidd[11], uuidd[12], uuidd[13], uuidd[14], uuidd[15]};
//     // vendor_config.proximity_uuid= {0xFD, 0xA5, 0x06, 0x93, 0xA4, 0xE2, 0x4F, 0xB1, 0xAF, 0xCF, 0xC6, 0xEB, 0x07, 0x64, 0x78, 0x25}
//     vendor_config.major = ENDIAN_CHANGE_U16(luaL_checkinteger(L, 2));
//     vendor_config.minor = ENDIAN_CHANGE_U16(luaL_checkinteger(L, 3));
//     vendor_config.measured_power = luaL_optinteger(L, 4, 0xC5);

//     esp_err_t status = esp_ble_config_ibeacon_data(&vendor_config, &ibeacon_adv_data);
//     if (status == ESP_OK)
//     {
//         esp_ble_gap_config_adv_data_raw((uint8_t *)&ibeacon_adv_data, sizeof(ibeacon_adv_data));
//         lua_pushboolean(L, 1);
//         return 0;
//     }
//     else
//     {
//         LLOGE("Config iBeacon data failed: %s\n", esp_err_to_name(status));
//         lua_pushboolean(L, 0);
//         return -1;
//     }
// }

// #include "rotable.h"
// static const rotable_Reg reg_ble[] =
//     {
//         {"adv_init", l_ble_adv_init, 0},
//         {"ibeacon_init", l_ble_ibeacon_init, 0},

//         {"ADV_TYPE_IND", NULL, 0x00},
//         {"ADV_TYPE_DIRECT_IND_HIGH", NULL, 0x01},
//         {"ADV_TYPE_SCAN_IND", NULL, 0x02},
//         {"ADV_TYPE_NONCONN_IND ", NULL, 0x03},
//         {"ADV_TYPE_DIRECT_IND_LOW", NULL, 0x04},

//         {"ADV_CHNL_37", NULL, 0x01},
//         {"ADV_CHNL_38", NULL, 0x02},
//         {"ADV_CHNL_39", NULL, 0x04},
//         {"ADV_CHNL_ALL", NULL, 0x07},

//         {"BLE_ADDR_TYPE_PUBLIC", NULL, 0x00},
//         {"BLE_ADDR_TYPE_RANDOM ", NULL, 0x01},
//         {"BLE_ADDR_TYPE_RPA_PUBLIC", NULL, 0x02},
//         {"BLE_ADDR_TYPE_RPA_RANDOM", NULL, 0x03},

//         {"ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY", NULL, 0x00},
//         {"ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY", NULL, 0x01},
//         {"ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST", NULL, 0x02},
//         {"ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST", NULL, 0x03},

//         {NULL, NULL, 0}};
// LUAMOD_API int luaopen_ble(lua_State *L)
// {
//     luat_newlib(L, reg_ble);
//     return 1;
// }
