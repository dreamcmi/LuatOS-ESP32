/*
 * SPDX-FileCopyrightText: 2022 zhuwangbin
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "luat_msgbus.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"


#define BLE_EVENT_BASE_MASK 0xffff0000
#define BLE_EVENT_GAP_BASE 0x10000
#define BLE_EVENT_GATTS_BASE 0x20000

#define BLE_EVENT_BASE_GET(event) (event&BLE_EVENT_BASE_MASK) 
#define BLE_EVENT_ID_GET(event) (event&(~BLE_EVENT_BASE_MASK))


typedef struct ble_cb_param_tag{
	int event;
	
	union {

		struct {
			esp_ble_gap_cb_param_t *p;
		} gap;
				
		struct {
			int gattsIf;
			esp_ble_gatts_cb_param_t *p;
		} gatts;
	} eventParam;
} ble_cb_param_t;


#define BLE_LOG_TAG "ble"

#define BLE_ERROR(fmt, ...) do {ESP_LOGE(BLE_LOG_TAG, "[%s,%d] "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__);}while(0)
#define BLE_DEBUG(fmt, ...) do {ESP_LOGE(BLE_LOG_TAG, "[%s,%d] "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__);}while(0)

extern int luat_ble_cb(lua_State *L, void* ptr);

#define BLE_RX_BUF_SIZE 2048

typedef struct {
    uint8_t *buf;
    uint32_t size;        
    uint32_t head;
    uint32_t tail;
    unsigned empty: 1;
    unsigned full:  1;
    unsigned overflow:  1;  
}CycleQueue;

static uint8_t g_s_bleRxBuff[BLE_RX_BUF_SIZE];
static CycleQueue g_s_bleQueue = {
	g_s_bleRxBuff,
	BLE_RX_BUF_SIZE,
	0,
	0,
	1,
	0,
	0,
};

static int prvQueueInsert(CycleQueue *Q_ptr, uint8_t *data, uint32_t len)
{
    uint32_t ret = 0;

    for (; ret < len; ret++)
    {
        *(Q_ptr->buf + Q_ptr->head) = *(data + ret);
        
        if ((1==Q_ptr->full) && (Q_ptr->head==Q_ptr->tail))
        {
            Q_ptr->overflow = 1;               
        }
        
        uint32_t temp =  ++Q_ptr->head % Q_ptr->size;
        Q_ptr->head = temp;
        if (Q_ptr->head == Q_ptr->tail)
        {
            Q_ptr->full = 1;
        }
        
        if (1 == Q_ptr->empty)
        {
            Q_ptr->empty = 0;
        }
    }
    
    if (Q_ptr->overflow)
    {
        Q_ptr->tail = Q_ptr->head;
    }
   
    return ret;       
}


static int prvQueueGetFreeSpace(CycleQueue *Q_ptr)
{
    if(Q_ptr->empty)
    {
        return Q_ptr->size;
    }
    if(Q_ptr->full || Q_ptr->overflow)
    {
      return 0;
    }
    
    if(Q_ptr->head > Q_ptr->tail)
    {
        return Q_ptr->size -(Q_ptr->head - Q_ptr->tail);
    }
    else
    {
        return Q_ptr->tail - Q_ptr->head;
    }
}

static int prvQueueLen(CycleQueue *Q_ptr)
{
    if(Q_ptr->empty)
    {
        return 0;
    }
    if(Q_ptr->full || Q_ptr->overflow)
    {
      return Q_ptr->size;
    }
    
    if(Q_ptr->head > Q_ptr->tail)
    {
        return (Q_ptr->head - Q_ptr->tail);
    }
    else
    {
        return Q_ptr->size - (Q_ptr->tail - Q_ptr->head);
    }
}


static int prvQueueDelete(CycleQueue *Q_ptr, uint8_t *data, uint32_t len)
{
    uint32_t ret = 0;
    
    if (!Q_ptr->empty)
    {
        while(ret < len) 
        {
            *(data + ret) = *(Q_ptr->buf + Q_ptr->tail);
            uint32_t temp = ++Q_ptr->tail % Q_ptr->size;
			Q_ptr->tail = temp;
            ret++;
            
            if (Q_ptr->tail == Q_ptr->head)
            {
                Q_ptr->empty = 1;
                break;
            }
        }
    }
   
    if ((ret>0) && (1==Q_ptr->full))   
    {
        Q_ptr->full = 0;
        Q_ptr->overflow = 0;
    }
    
    return ret;    
}

static void prvBleGattsEvent(esp_gatts_cb_event_t event, esp_gatt_if_t gattsIf, esp_ble_gatts_cb_param_t *param)
{
	ble_cb_param_t *cbParam;
	rtos_msg_t msg = {0};

	cbParam = malloc(sizeof(ble_cb_param_t));
	//todo,ESP_ERROR_CHECK((cbParam) ? ESP_OK : ESP_FAIL);
	
	cbParam->eventParam.gatts.p = malloc(sizeof(esp_ble_gatts_cb_param_t));
	//todo,ESP_ERROR_CHECK((cbParam->eventParam.gatts.p) ? ESP_OK : ESP_FAIL);
	
	cbParam->event = event + BLE_EVENT_GATTS_BASE;
	cbParam->eventParam.gatts.gattsIf = gattsIf;
	memcpy(cbParam->eventParam.gatts.p, param, sizeof(esp_ble_gatts_cb_param_t));

	switch(event)
	{
		case ESP_GATTS_WRITE_EVT:
		{
			if (prvQueueGetFreeSpace(&g_s_bleQueue) > sizeof(esp_ble_gatts_cb_param_t))
			{
				esp_ble_gatts_cb_param_t writeParam = {0};

				memcpy(&writeParam, param, sizeof(writeParam));

				writeParam.write.value = malloc(writeParam.write.len);
				memcpy(writeParam.write.value, param->write.value, writeParam.write.len);
				prvQueueInsert(&g_s_bleQueue, (uint8_t *)&writeParam, sizeof(writeParam));
			}
				
			break;
		}
		default:
			break;
	}

	msg.handler = luat_ble_cb;
	msg.ptr = cbParam;
	luat_msgbus_put(&msg, 0);
}

static void prvBleGapEvent(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	ble_cb_param_t *cbParam;
	rtos_msg_t msg = {0};

	cbParam = malloc(sizeof(ble_cb_param_t));
	//todo,ESP_ERROR_CHECK((cbParam) ? ESP_OK : ESP_FAIL);
	
	cbParam->eventParam.gap.p = malloc(sizeof(esp_ble_gap_cb_param_t));
	//todo,ESP_ERROR_CHECK((cbParam->eventParam.gap.p) ? ESP_OK : ESP_FAIL);
	
	cbParam->event = event + BLE_EVENT_GAP_BASE;
	memcpy(cbParam->eventParam.gap.p, param, sizeof(esp_ble_gatts_cb_param_t));

	msg.handler = luat_ble_cb;
	msg.ptr = cbParam;
	luat_msgbus_put(&msg, 0);
}

void bleEvevntParamFree(void *ptr)
{
	ble_cb_param_t *param = (ble_cb_param_t *)ptr;
	int eventBase = BLE_EVENT_BASE_GET(param->event);

	if (BLE_EVENT_GAP_BASE == eventBase)
	{
		esp_ble_gap_cb_param_t *gap = param->eventParam.gap.p;

		free(gap);
	}
	else if (BLE_EVENT_GATTS_BASE == eventBase)
	{
		esp_ble_gatts_cb_param_t *gatts = param->eventParam.gatts.p;

		free(gatts);
	}

	free(ptr);
}

esp_err_t bleInit(void)
{
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	esp_err_t ret;
	
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        BLE_ERROR("%s init controller failed: %s", __func__, esp_err_to_name(ret));
        return ret;
    }
	
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        BLE_ERROR("%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return ret;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        BLE_ERROR("%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return ret;
    }
	
    ret = esp_bluedroid_enable();
    if (ret) {
        BLE_ERROR("%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return ret;
    }

    ret = esp_ble_gatts_register_callback(prvBleGattsEvent);
    if (ret){
        BLE_ERROR("gatts register error, error code = %x", ret);
        return ret;
    }
    ret = esp_ble_gap_register_callback(prvBleGapEvent);
    if (ret){
        BLE_ERROR("gap register error, error code = %x", ret);
        return ret;
    }
	
	return ret;
}

esp_err_t bleGattsAppRegister(uint16_t appId)
{
	return esp_ble_gatts_app_register(appId);
}


esp_err_t bleGattsCreateService(esp_gatt_if_t gattsIf, uint8_t *uuid, uint16_t uuidLen, uint16_t attrNum)
{
	esp_gatt_srvc_id_t serviceId;
	serviceId.is_primary = true;
	serviceId.id.inst_id = 0x00;
	serviceId.id.uuid.len = uuidLen;
	
	memcpy(&serviceId.id.uuid.uuid, uuid, uuidLen);
    return esp_ble_gatts_create_service(gattsIf, &serviceId, attrNum+1);
}


esp_err_t bleGattsStartService(uint16_t serviceHandle)
{
	return esp_ble_gatts_start_service(serviceHandle);
}

esp_err_t bleGattsAddChar(uint16_t serviceHandle, uint8_t *uuid, uint16_t uuidLen, uint8_t *charVal, uint16_t charValLen)
{
	esp_bt_uuid_t charUuid;
	esp_attr_value_t attrCharVal;

	charUuid.len = uuidLen;
	memcpy(&charUuid.uuid, uuid, uuidLen);
	attrCharVal.attr_max_len = charValLen;
	attrCharVal.attr_len = charValLen;
	attrCharVal.attr_value = charVal;

	return esp_ble_gatts_add_char(serviceHandle, &charUuid, 
				ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, 
				(ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY), 
				&attrCharVal, NULL);
}

esp_err_t bleGattsAddCharDescr(uint16_t serviceHandle, uint8_t *uuid, uint16_t uuidLen, uint8_t *charDescrVal, uint16_t charDescrValLen)
{
	esp_bt_uuid_t descrUuid;
	esp_attr_value_t attrCharDescrVal;

    descrUuid.len = uuidLen;
	memcpy(&descrUuid.uuid, uuid, uuidLen);

	attrCharDescrVal.attr_max_len = charDescrValLen;
	attrCharDescrVal.attr_len = charDescrValLen;
	attrCharDescrVal.attr_value = charDescrVal;
	
    return esp_ble_gatts_add_char_descr(serviceHandle, &descrUuid,
                              ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, &attrCharDescrVal, NULL);
}


esp_err_t bleGattsSendResponse(esp_gatt_if_t gattsIf, uint16_t attrHandle, uint16_t connId, uint32_t transId, uint8_t *rspVal, uint8_t rspValLen)
{
	esp_gatt_rsp_t *rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
	
	memset(rsp, 0, sizeof(esp_gatt_rsp_t));

	rsp->attr_value.handle = attrHandle;
	rsp->attr_value.len = rspValLen;
	memcpy(rsp->attr_value.value, rspVal, rspValLen);
	return esp_ble_gatts_send_response(gattsIf, connId, transId, ESP_GATT_OK, rsp);
}

esp_err_t bleGattsRead(esp_ble_gatts_cb_param_t *param)
{
	if (prvQueueLen(&g_s_bleQueue) >= sizeof(esp_ble_gatts_cb_param_t))
	{
		prvQueueDelete(&g_s_bleQueue, (uint8_t * )param, sizeof(esp_ble_gatts_cb_param_t));
		return ESP_GATT_OK;
	}
	
	return ESP_GATT_ERROR;
}



/*gap 接口*/

esp_err_t bleGapExtAdvSetParams(uint8_t instance, uint32_t interval_min, uint32_t interval_max)
{
	esp_ble_gap_ext_adv_params_t ext_adv_params_1M = {
		    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE,
		    .interval_min = interval_min,
		    .interval_max = interval_max,
		    .channel_map = ADV_CHNL_ALL,
		    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
		    .primary_phy = ESP_BLE_GAP_PHY_1M,
		    .max_skip = 0,
		    .secondary_phy = ESP_BLE_GAP_PHY_1M,
		    .sid = 0,
		    .scan_req_notif = false,
		    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
		};
	return esp_ble_gap_ext_adv_set_params(instance, &ext_adv_params_1M);
}

esp_err_t bleGapConfigExtAdvDataRaw(uint8_t instance, uint16_t length, const uint8_t *data)
{
	return esp_ble_gap_config_ext_adv_data_raw(instance, length, data);
}

esp_err_t bleGapExtAdvSetRandAddr(uint8_t instance, esp_bd_addr_t rand_addr)
{
	return esp_ble_gap_ext_adv_set_rand_addr(instance, rand_addr);
}

esp_err_t bleGapExtAdvStart(uint8_t instanceNum)
{
	esp_ble_gap_ext_adv_t *ext_adv = malloc(instanceNum * sizeof(esp_ble_gap_ext_adv_t));
	//todo,ESP_ERROR_CHECK((ext_adv) ? ESP_OK : ESP_FAIL);
	memset(ext_adv, 0, (instanceNum * sizeof(esp_ble_gap_ext_adv_t)));

	for (int i=0; i<instanceNum; i++)
	{
		ext_adv[i].instance = i;
	}
	
	return esp_ble_gap_ext_adv_start(instanceNum, ext_adv);
}

typedef struct luat_uart_cb {
    int eventCb; //蓝牙回调函数
} luat_ble_cb_t;

static luat_ble_cb_t bt_evevtCb;

char *gatts_string[] = {
	"MSG_GATTS_REG_EVT",				 // 0,		 /*!< When register application id, the event comes */
	"MSG_GATTS_READ_EVT",				 // 1,		 /*!< When gatt client request read operation, the event comes */
	"MSG_GATTS_WRITE_EVT",				 // 2,		 /*!< When gatt client request write operation, the event comes */
	"MSG_GATTS_EXEC_WRITE_EVT", 		 // 3,		 /*!< When gatt client request execute write, the event comes */
	"MSG_GATTS_MTU_EVT",				 // 4,		 /*!< When set mtu complete, the event comes */
	"MSG_GATTS_CONF_EVT",				 // 5,		 /*!< When receive confirm, the event comes */
	"MSG_GATTS_UNREG_EVT",				 // 6,		 /*!< When unregister application id, the event comes */
	"MSG_GATTS_CREATE_EVT", 			 // 7,		 /*!< When create service complete, the event comes */
	"MSG_GATTS_ADD_INCL_SRVC_EVT",		 // 8,		 /*!< When add included service complete, the event comes */
	"MSG_GATTS_ADD_CHAR_EVT",			 // 9,		 /*!< When add characteristic complete, the event comes */
	"MSG_GATTS_ADD_CHAR_DESCR_EVT", 	 // 10, 	 /*!< When add descriptor complete, the event comes */
	"MSG_GATTS_DELETE_EVT", 			 // 11, 	 /*!< When delete service complete, the event comes */
	"MSG_GATTS_START_EVT",				 // 12, 	 /*!< When start service complete, the event comes */
	"MSG_GATTS_STOP_EVT",				 // 13, 	 /*!< When stop service complete, the event comes */
	"MSG_GATTS_CONNECT_EVT",			 // 14, 	 /*!< When gatt client connect, the event comes */
	"MSG_GATTS_DISCONNECT_EVT", 		 // 15, 	 /*!< When gatt client disconnect, the event comes */
	"MSG_GATTS_OPEN_EVT",				 // 16, 	 /*!< When connect to peer, the event comes */
	"MSG_GATTS_CANCEL_OPEN_EVT",		 // 17, 	 /*!< When disconnect from peer, the event comes */
	"MSG_GATTS_CLOSE_EVT",				 // 18, 	 /*!< When gatt server close, the event comes */
	"MSG_GATTS_LISTEN_EVT", 			 // 19, 	 /*!< When gatt listen to be connected the event comes */
	"MSG_GATTS_CONGEST_EVT",			 // 20, 	 /*!< When congest happen, the event comes */
	"MSG_GATTS_RMSGONSE_EVT",			 // 21, 	 /*!< When gatt send rMSGonse complete, the event comes */
	"MSG_GATTS_CREAT_ATTR_TAB_EVT", 	 // 22, 	 /*!< When gatt create table complete, the event comes */
	"MSG_GATTS_SET_ATTR_VAL_EVT",		 // 23, 	 /*!< When gatt set attr value complete, the event comes */
	"MSG_GATTS_SEND_SERVICE_CHANGE_EVT", // 24,

};

char *gap_string[] = {
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    "MSG_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT",//        = 0,       /*!< When advertising data set complete, the event comes */
    "MSG_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT",//,             /*!< When scan rMSGonse data set complete, the event comes */
    "MSG_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT",//,                /*!< When scan parameters set complete, the event comes */
    "MSG_GAP_BLE_SCAN_RESULT_EVT",//,                            /*!< When one scan result ready, the event comes each time */
    "MSG_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT",//,              /*!< When raw advertising data set complete, the event comes */
    "MSG_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT",//,         /*!< When raw advertising data set complete, the event comes */
    "MSG_GAP_BLE_ADV_START_COMPLETE_EVT",//,                     /*!< When start advertising complete, the event comes */
    "MSG_GAP_BLE_SCAN_START_COMPLETE_EVT",//,                    /*!< When start scan complete, the event comes */
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    "MSG_GAP_BLE_AUTH_CMPL_EVT",// = 8,                          /* Authentication complete indication. */
    "MSG_GAP_BLE_KEY_EVT",//,                                    /* BLE  key event for peer device keys */
    "MSG_GAP_BLE_SEC_REQ_EVT",//,                                /* BLE  security request */
    "MSG_GAP_BLE_PASSKEY_NOTIF_EVT",//,                          /* passkey notification event */
    "MSG_GAP_BLE_PASSKEY_REQ_EVT",//,                            /* passkey request event */
    "MSG_GAP_BLE_OOB_REQ_EVT",//,                                /* OOB request event */
    "MSG_GAP_BLE_LOCAL_IR_EVT",//,                               /* BLE local IR event */
    "MSG_GAP_BLE_LOCAL_ER_EVT",//,                               /* BLE local ER event */
    "MSG_GAP_BLE_NC_REQ_EVT",//,                                 /* Numeric Comparison request event */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    "MSG_GAP_BLE_ADV_STOP_COMPLETE_EVT",//,                      /*!< When stop adv complete, the event comes */
    "MSG_GAP_BLE_SCAN_STOP_COMPLETE_EVT",//,                     /*!< When stop scan complete, the event comes */
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    "MSG_GAP_BLE_SET_STATIC_RAND_ADDR_EVT",// = 19,              /*!< When set the static rand address complete, the event comes */
    "MSG_GAP_BLE_UPDATE_CONN_PARAMS_EVT",//,                     /*!< When update connection parameters complete, the event comes */
    "MSG_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT",//,                /*!< When set pkt length complete, the event comes */
    "MSG_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT",//,             /*!< When  Enable/disable privacy on the local device complete, the event comes */
    "MSG_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT",//,               /*!< When remove the bond device complete, the event comes */
    "MSG_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT",//,                /*!< When clear the bond device clear complete, the event comes */
    "MSG_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT",//,                  /*!< When get the bond device list complete, the event comes */
    "MSG_GAP_BLE_READ_RSSI_COMPLETE_EVT",//,                     /*!< When read the rssi complete, the event comes */
    "MSG_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT",//,              /*!< When add or remove whitelist complete, the event comes */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    "MSG_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT",//,  /*!< When update duplicate exceptional list complete, the event comes */
#endif //#if (BLE_42_FEATURE_SUPPORT == TRUE)
    "MSG_GAP_BLE_SET_CHANNELS_EVT",// = 29,                           /*!< When setting BLE channels complete, the event comes */
    "MSG_GAP_BLE_READ_PHY_COMPLETE_EVT",//,
    "MSG_GAP_BLE_SET_PREFERED_DEFAULT_PHY_COMPLETE_EVT",//,
    "MSG_GAP_BLE_SET_PREFERED_PHY_COMPLETE_EVT",//,
    "MSG_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT",//,
    "MSG_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT",//,34
    "MSG_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT",//,35
    "MSG_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT",//,
    "MSG_GAP_BLE_EXT_ADV_START_COMPLETE_EVT",//,37
    "MSG_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT",//,
    "MSG_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT",//,
    "MSG_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT",//,
    "MSG_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT",//,
    "MSG_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT",//,
    "MSG_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT",//,
    "MSG_GAP_BLE_PHY_UPDATE_COMPLETE_EVT",//,
    "MSG_GAP_BLE_EXT_ADV_REPORT_EVT",//,
    "MSG_GAP_BLE_SCAN_TIMEOUT_EVT",//,
    "MSG_GAP_BLE_ADV_TERMINATED_EVT",//,
    "MSG_GAP_BLE_SCAN_REQ_RECEIVED_EVT",//,
    "MSG_GAP_BLE_CHANNEL_SELETE_ALGORITHM_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_REPORT_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT",//,
    "MSG_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT",//,
    "MSG_GAP_BLE_EVT",//_MAX,
};



int luat_ble_cb(lua_State *L, void* ptr)
{
	ble_cb_param_t *param = (ble_cb_param_t *)ptr;
	int eventBase = BLE_EVENT_BASE_GET(param->event);
	int eventId = BLE_EVENT_ID_GET(param->event);
	
	if (bt_evevtCb.eventCb)
	{	
		lua_geti(L, LUA_REGISTRYINDEX, bt_evevtCb.eventCb);
		if (!lua_isfunction(L, -1))
		{
			//todo,ESP_ERROR_CHECK(ESP_FAIL);
		}
	}
	else
	{
		//todo,ESP_ERROR_CHECK(ESP_FAIL);
	}
	
	if (BLE_EVENT_GAP_BASE == eventBase)
	{
		esp_ble_gap_cb_param_t *gap = param->eventParam.gap.p;

		//LUAT_BLE_DEBUG("gap event id %d string %s, status %d", eventId, gap_string[eventId], gap->adv_data_cmpl.status);
		/*gap 消息至少2个参数*/
		lua_pushinteger(L, param->event); 
		lua_pushinteger(L, gap->adv_data_cmpl.status);
				
		switch(eventId)
		{
			case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT://= 0,/*!< When advertising data set complete, the event comes */
			case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT: //34
			case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT: //35
			case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:	//37		
			default:
				
				lua_call(L, 2, 0);
				break;
		}
	}
	else if (BLE_EVENT_GATTS_BASE == eventBase)
	{
		esp_ble_gatts_cb_param_t *gatts = param->eventParam.gatts.p;
		int gattsIf = param->eventParam.gatts.gattsIf;
		//LUAT_BLE_DEBUG("gatts event id %d string %s, status %d", eventId, gatts_string[eventId], gatts->reg.status);
		
		/*gatts 消息至少3个参数*/
		lua_pushinteger(L, param->event); 
		/*部分消息没有status，默认上传ESP_GATT_OK*/
		switch(eventId)
		{
			case ESP_GATTS_READ_EVT:
			case ESP_GATTS_WRITE_EVT:
			case ESP_GATTS_EXEC_WRITE_EVT:
			case ESP_GATTS_MTU_EVT:	
			case ESP_GATTS_CONNECT_EVT:
			case ESP_GATTS_DISCONNECT_EVT:
			case ESP_GATTS_CONGEST_EVT:
				lua_pushinteger(L, ESP_GATT_OK);
				break;
			case ESP_GATTS_SET_ATTR_VAL_EVT:
				lua_pushinteger(L, gatts->set_attr_val.status);
				break;
			default:
				lua_pushinteger(L, gatts->reg.status);
				break;
		}
		
		lua_pushinteger(L, gattsIf);
		
		switch(eventId)
		{				
			case ESP_GATTS_CREATE_EVT: 
			{
	            if (gatts->add_attr_tab.status == ESP_GATT_OK){
					lua_pushinteger(L, gatts->create.service_handle);
					lua_call(L, 4, 0);
				}
				else {
					lua_call(L, 3, 0);
				}
				break;
			}
			case ESP_GATTS_ADD_CHAR_EVT:
			{
				if (gatts->add_char.status == ESP_GATT_OK){
					lua_pushinteger(L, gatts->add_char.attr_handle);
					lua_call(L, 4, 0);
				}
				else {
					lua_call(L, 3, 0);
				}
				break;
			}
			case ESP_GATTS_ADD_CHAR_DESCR_EVT:
			{
				if (gatts->add_char_descr.status == ESP_GATT_OK){
					lua_pushinteger(L, gatts->add_char_descr.attr_handle);
					lua_call(L, 4, 0);
				}
				else {
					lua_call(L, 3, 0);
				}
				break;
			}	
			case ESP_GATTS_READ_EVT:
			{
				lua_pushinteger(L, gatts->read.handle);
				lua_pushinteger(L, gatts->read.conn_id);
				lua_pushinteger(L, gatts->read.trans_id);
				lua_call(L, 6, 0);
				break;
			}	
			case ESP_GATTS_WRITE_EVT:
			{
				lua_pushinteger(L, gatts->write.handle);
				lua_pushinteger(L, gatts->write.conn_id);
				lua_pushinteger(L, gatts->write.trans_id);
				lua_call(L, 6, 0);
				break;
			}	
			case ESP_GATTS_CONNECT_EVT:
			{
				lua_pushinteger(L, gatts->connect.conn_id);
				lua_call(L, 4, 0);
				break;
			}
			case ESP_GATTS_DISCONNECT_EVT:
			{
				lua_pushinteger(L, gatts->disconnect.conn_id);
				lua_call(L, 4, 0);
				break;
			}
			
			case ESP_GATTS_REG_EVT://= 0,/*!< When register application id, the event comes */
			default:
				
				lua_call(L, 3, 0);
				break;
		}
	}
	else
	{
		//todo,ESP_ERROR_CHECK(ESP_FAIL);
	}

	bleEvevntParamFree(ptr);
	return 0;
}

/*
初始化ble
@api ble.init()
@return int  esp_err 成功0
@usage 
ble.init()
*/
static int l_ble_init(lua_State *L)
{
	esp_err_t ret;

	if (lua_isfunction(L, 1)) 
	{
		lua_pushvalue(L, 1);
		bt_evevtCb.eventCb = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	else
	{
		//todo,ESP_ERROR_CHECK(ESP_FAIL);
	}

	ret = bleInit();
	
	lua_pushinteger(L, ret);

    return 1;
}

/*
去初始化ble
@api ble.deinit()
@return int  esp_err 成功0
@usage 
ble.deinit()
*/
static int l_ble_deinit(lua_State *L)
{
    esp_err_t err = ESP_FAIL;
    esp_bluedroid_disable(); //todo error check
    esp_bluedroid_deinit(); //todo error check
     esp_bt_controller_disable(); //todo error check
    err = esp_bt_controller_deinit();
    lua_pushinteger(L, err);
    return 1;
}

/*
注册应用程序
@api ble.gatts_app_regist()
@return int  esp_err 成功0
@usage 
ble.l_gatts_app_regist()
*/
static int l_gatts_app_regist(lua_State *L)
{
	esp_err_t ret;
	
	int appId = luaL_checkinteger(L, 1);
	
	ret = bleGattsAppRegister((uint16_t)appId);
	
	lua_pushinteger(L, ret);
	
    return 1;
}

/*
添加服务
@api ble.gatts_create_server()
@return int  esp_err 成功0
@usage 
ble.gatts_create_server()
*/
static int l_gatts_create_service(lua_State *L)
{
	esp_err_t ret;
	int len, gattsIf, attrNum; 
	char *uuid;	
	
	gattsIf = luaL_checkinteger(L, 1);
	attrNum = luaL_checkinteger(L, 3);
	len = lua_rawlen(L, 2);
	//todo,ESP_ERROR_CHECK((len<=8)?ESP_OK:ESP_FAIL);

	uuid = (char *)malloc(len);
	//todo,ESP_ERROR_CHECK((uuid)?ESP_OK:ESP_FAIL);
	
	for (size_t i = 0; i < len; i++)
    {
        lua_rawgeti(L, 2, 1 + i);
        uuid[i] = (char)lua_tointeger(L, -1);
        lua_pop(L, 1); 
    }
	
	ret = bleGattsCreateService((esp_gatt_if_t)gattsIf, (uint8_t *)uuid, (uint16_t)len, (uint16_t)attrNum);

	free(uuid);
	
	lua_pushinteger(L, ret);
	
    return 1;
}

/*
开始服务
@api ble.gatts_start_service()
@return int  esp_err 成功0
@usage 
ble.gatts_start_service()
*/
static int l_gatts_start_service(lua_State *L)
{
	esp_err_t ret;
	int serviceHandle;	
	
	serviceHandle = luaL_checkinteger(L, 1);
	
	ret = bleGattsStartService(serviceHandle);
	
	lua_pushinteger(L, ret);
	
    return 1;
}

/*
添加特征
@api ble.gatts_add_char()
@return int  esp_err 成功0
@usage 
ble.gatts_add_char()
*/
static int l_gatts_add_char(lua_State *L)
{
	esp_err_t ret;
	char *uuid, *charVal;	
	int uuidLen, charValLen, serviceHandle; 
	
	serviceHandle = luaL_checkinteger(L, 1);
	uuidLen = lua_rawlen(L, 2);
	//todo,ESP_ERROR_CHECK((uuidLen<=8)?ESP_OK:ESP_FAIL);

	uuid = (char *)malloc(uuidLen);
	//todo,ESP_ERROR_CHECK((uuid)?ESP_OK:ESP_FAIL);
	for (size_t i = 0; i < uuidLen; i++)
    {
        lua_rawgeti(L, 2, 1 + i);
        uuid[i] = (char)lua_tointeger(L, -1);
        lua_pop(L, 1); 
    }

	charValLen = lua_rawlen(L, 3);
	charVal = (char *)malloc(charValLen);
	//todo,ESP_ERROR_CHECK((charVal)?ESP_OK:ESP_FAIL);
	
	for (size_t i = 0; i < charValLen; i++)
    {
        lua_rawgeti(L, 3, 1 + i);
        charVal[i] = (char)lua_tointeger(L, -1);
        lua_pop(L, 1); 
    }
	
	ret = bleGattsAddChar((uint16_t) serviceHandle, (uint8_t *) uuid, (uint16_t) uuidLen, (uint8_t *) charVal, (uint16_t) charValLen);

	free(uuid);
	free(charVal);
	
	lua_pushinteger(L, ret);
	
    return 1;
}

/*
添加描述
@api ble.gatts_add_char_descr()
@return int  esp_err 成功0
@usage 
ble.gatts_add_char_descr()
*/
static int l_gatts_add_char_descr(lua_State *L)
{
	esp_err_t ret;
	char *uuid, *charDescrVal;	
	int uuidLen, charDescrValLen, serviceHandle; 
	
	serviceHandle = luaL_checkinteger(L, 1);

	uuidLen = lua_rawlen(L, 2);
	//todo,ESP_ERROR_CHECK((uuidLen<=8)?ESP_OK:ESP_FAIL);

	uuid = (char *)malloc(uuidLen);
	//todo,ESP_ERROR_CHECK((uuid)?ESP_OK:ESP_FAIL);
	
	for (size_t i = 0; i < uuidLen; i++)
    {
        lua_rawgeti(L, 2, 1 + i);
        uuid[i] = (char)lua_tointeger(L, -1);
        lua_pop(L, 1); 
    }
	
	charDescrValLen = lua_rawlen(L, 3);
	charDescrVal = (char *)malloc(charDescrValLen);
	//todo,ESP_ERROR_CHECK((charDescrVal)?ESP_OK:ESP_FAIL);
	
	for (size_t i = 0; i < charDescrValLen; i++)
    {
        lua_rawgeti(L, 3, 1 + i);
        charDescrVal[i] = (char)lua_tointeger(L, -1);
        lua_pop(L, 1); 
    }
	
	ret = bleGattsAddCharDescr((uint16_t) serviceHandle, (uint8_t *) uuid, (uint16_t) uuidLen, (uint8_t *) charDescrVal, (uint16_t) charDescrValLen);

	free(uuid);
	free(charDescrVal);
	
	lua_pushinteger(L, ret);
	
    return 1;
}

static int l_gatts_send_response(lua_State *L)
{
	esp_err_t ret;
	char *rspVal;	
	int attrHandle, gattsIf, connId, transId, rspValLen; 
	
	gattsIf = luaL_checkinteger(L, 1);
	attrHandle = luaL_checkinteger(L, 2);
	connId = luaL_checkinteger(L, 3);
	transId = luaL_checkinteger(L, 4);
	
	rspValLen = lua_rawlen(L, 5);
	//todo,ESP_ERROR_CHECK((rspValLen<ESP_GATT_MAX_ATTR_LEN)?ESP_OK:ESP_FAIL);
	rspVal = (char *)malloc(rspValLen);
	//todo,ESP_ERROR_CHECK((rspVal)?ESP_OK:ESP_FAIL);
	
	for (size_t i = 0; i < rspValLen; i++)
    {
        lua_rawgeti(L, 5, 1 + i);
        rspVal[i] = (char)lua_tointeger(L, -1);
        lua_pop(L, 1); 
    }

	ret = bleGattsSendResponse((esp_gatt_if_t) gattsIf, (uint16_t) attrHandle, (uint16_t) connId, (uint32_t) transId, (uint8_t *) rspVal, (uint8_t) rspValLen);

	free(rspVal);
	
	lua_pushinteger(L, ret);
	
    return 1;
}

static int l_gatts_read(lua_State *L)
{
	esp_ble_gatts_cb_param_t param;
	esp_err_t ret = bleGattsRead(&param);

	if (ret)
	{
		lua_pushinteger(L, ret);
		return 1;
	}
	lua_pushinteger(L, ret);
	lua_pushinteger(L, param.read.handle);
	lua_pushinteger(L, param.read.conn_id);
	lua_pushinteger(L, param.read.trans_id);
	lua_pushlstring(L, (const char*)param.write.value, param.write.len);
	free(param.write.value);
    return 5;
}

/*
设置广播发布参数，广播间隔
@api ble.l_gap_ext_adv_set_params()
@return int  esp_err 成功0
@usage 
ble.l_gap_ext_adv_set_params()
*/
static int l_gap_ext_adv_set_params(lua_State *L)
{
	esp_err_t ret;
	int instance, interval_min, interval_max;

	instance = luaL_checkinteger(L, 1);
	interval_min = luaL_checkinteger(L, 2);
	interval_max = luaL_checkinteger(L, 3);
	
	ret = bleGapExtAdvSetParams(instance, interval_min, interval_max);

	lua_pushinteger(L, ret);
	
    return 1;
}

/*
设置设备随机地址
@api ble.l_gap_ext_adv_set_rand_addr()
@return int  esp_err 成功0
@usage 
ble.l_gap_ext_adv_set_rand_addr()
*/
static int l_gap_ext_adv_set_rand_addr(lua_State *L)
{
	esp_err_t ret;
	int instance;
	int len; 
	esp_bd_addr_t rand_addr;
		
	instance = luaL_checkinteger(L, 1);
	
	len = lua_rawlen(L, 2);
	//todo,ESP_ERROR_CHECK((len==6)?ESP_OK:ESP_FAIL);
	
    char *mac = (char *)malloc(len);
	//todo,ESP_ERROR_CHECK((mac)?ESP_OK:ESP_FAIL);
	
    for (size_t i = 0; i < len; i++)
    {
        lua_rawgeti(L, 2, 1 + i);
        mac[i] = (char)lua_tointeger(L, -1);
        lua_pop(L, 1); 
    }

	memcpy(&rand_addr, mac, sizeof(rand_addr));
	ret = bleGapExtAdvSetRandAddr(instance, rand_addr);

	free(mac);
	
	lua_pushinteger(L, ret);
	
    return 1;
}



/*
配置广播数据
@api ble.l_gap_config_ext_adv_data_raw()
@return int  esp_err 成功0
@usage 
ble.l_gap_config_ext_adv_data_raw()
*/
static int l_gap_config_ext_adv_data_raw(lua_State *L)
{
	esp_err_t ret;
	int instance;
	int len; //返回数组的长度
    char *data;

	instance = luaL_checkinteger(L, 1);
	
	len = lua_rawlen(L, 2); 

    data = (char *)malloc(len);
	//todo,ESP_ERROR_CHECK((data)?ESP_OK:ESP_FAIL);
	
    for (size_t i = 0; i < len; i++)
    {
        lua_rawgeti(L, 2, 1 + i);
        data[i] = (char)lua_tointeger(L, -1);
        lua_pop(L, 1); 
    }

	ret = bleGapConfigExtAdvDataRaw(instance, len,
						(const uint8_t *)data);

	free(data);
	
	lua_pushinteger(L, ret);
	
    return 1;
}

/*
开始广播
@api ble.l_gap_ext_adv_start()
@return int  esp_err 成功0
@usage 
ble.l_gap_ext_adv_start()
*/
static int l_gap_ext_adv_start(lua_State *L)
{
	esp_err_t ret;
	int instanceNum;
	
	instanceNum = luaL_checkinteger(L, 1);
	
	ret = bleGapExtAdvStart(instanceNum);
	
	lua_pushinteger(L, ret);
	
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_ble[] =
{
    {"init", l_ble_init, 0},

	/*GATTS 接口*/
	{"gattsAppRegist", l_gatts_app_regist, 0},
    {"gattsCreateService", l_gatts_create_service, 0},
    {"gattsStartService", l_gatts_start_service, 0},
    {"gattsAddChar", l_gatts_add_char, 0},
    {"gattsAddCharDescr", l_gatts_add_char_descr, 0},
    {"gattsSendResponse", l_gatts_send_response, 0},
    {"gattsRead", l_gatts_read, 0},

	/*GAP 接口*/
    {"gapExtAdvSetParams", l_gap_ext_adv_set_params, 0},
    {"gapExtAdvSetRandAddr",l_gap_ext_adv_set_rand_addr,0},
    {"gapConfigExtAdvDataRaw", l_gap_config_ext_adv_data_raw, 0},
    {"gapExtAdvStart", l_gap_ext_adv_start, 0},

    {"deinit", l_ble_deinit, 0},
    {NULL, NULL, 0}
};
	
LUAMOD_API int luaopen_ble(lua_State *L)
{
	int i;
	int gapMsgNum = sizeof(gap_string)/sizeof(char *);
	int gattsMsgNum = sizeof(gatts_string)/sizeof(char *);
	int regBleNum = sizeof(reg_ble)/sizeof(rotable_Reg);
	
	rotable_Reg *table = malloc((gapMsgNum+gattsMsgNum+regBleNum)*sizeof(rotable_Reg));
	//todo,ESP_ERROR_CHECK((table)?ESP_OK:ESP_FAIL);
	memset(table, 0, (gapMsgNum+gattsMsgNum+regBleNum)*sizeof(rotable_Reg));
	
	for (i=0; i<gattsMsgNum; i++)
	{
		table[i].name = gatts_string[i];
		table[i].value = i+BLE_EVENT_GATTS_BASE;
	}
	
	for (i=0; i<gapMsgNum; i++)
	{
		table[i+gattsMsgNum].name = gap_string[i];
		table[i+gattsMsgNum].value = i+BLE_EVENT_GAP_BASE;
	}

	memcpy(&table[gattsMsgNum+gapMsgNum], reg_ble, sizeof(reg_ble));

    luat_newlib(L, table);

    return 1;
}

