PROJECT = "ble"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

--MSG GATTS 从0x20000开始
local msg_gatts = {
	"MSG_GATTS_REG_EVT",				 -- 0x20000,		 /*!< When register application id, the event comes */
	"MSG_GATTS_READ_EVT",				 --     /*!< When gatt client request read operation, the event comes */
	"MSG_GATTS_WRITE_EVT",				 -- 	/*!< When gatt client request write operation, the event comes */
	"MSG_GATTS_EXEC_WRITE_EVT", 		 -- 	/*!< When gatt client request execute write, the event comes */
	"MSG_GATTS_MTU_EVT",				 -- 	/*!< When set mtu complete, the event comes */
	"MSG_GATTS_CONF_EVT",				 -- 	/*!< When receive confirm, the event comes */
	"MSG_GATTS_UNREG_EVT",				 -- 	/*!< When unregister application id, the event comes */
	"MSG_GATTS_CREATE_EVT", 			 -- 	/*!< When create service complete, the event comes */
	"MSG_GATTS_ADD_INCL_SRVC_EVT",		 -- 	/*!< When add included service complete, the event comes */
	"MSG_GATTS_ADD_CHAR_EVT",			 -- 	/*!< When add characteristic complete, the event comes */
	"MSG_GATTS_ADD_CHAR_DESCR_EVT", 	 --  	 /*!< When add descriptor complete, the event comes */
	"MSG_GATTS_DELETE_EVT", 			 --  	 /*!< When delete service complete, the event comes */
	"MSG_GATTS_START_EVT",				 --  	 /*!< When start service complete, the event comes */
	"MSG_GATTS_STOP_EVT",				 --  	 /*!< When stop service complete, the event comes */
	"MSG_GATTS_CONNECT_EVT",			 -- 	 /*!< When gatt client connect, the event comes */
	"MSG_GATTS_DISCONNECT_EVT", 		 --  	 /*!< When gatt client disconnect, the event comes */
	"MSG_GATTS_OPEN_EVT",				 --  	 /*!< When connect to peer, the event comes */
	"MSG_GATTS_CANCEL_OPEN_EVT",		 --  	 /*!< When disconnect from peer, the event comes */
	"MSG_GATTS_CLOSE_EVT",				 --  	 /*!< When gatt server close, the event comes */
	"MSG_GATTS_LISTEN_EVT", 			 --  	 /*!< When gatt listen to be connected the event comes */
	"MSG_GATTS_CONGEST_EVT",			 --  	 /*!< When congest happen, the event comes */
	"MSG_GATTS_RMSGONSE_EVT",			 -- 	 /*!< When gatt send rMSGonse complete, the event comes */
	"MSG_GATTS_CREAT_ATTR_TAB_EVT", 	 --  	 /*!< When gatt create table complete, the event comes */
	"MSG_GATTS_SET_ATTR_VAL_EVT",		 -- 	 /*!< When gatt set attr value complete, the event comes */
	"MSG_GATTS_SEND_SERVICE_CHANGE_EVT", -- 
};

--MSG GAP 从0x10000开始
local msg_gap = {
    "MSG_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT",--       = 0x10000,       /*!< When advertising data set complete, the event comes */
    "MSG_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT",--,             /*!< When scan rMSGonse data set complete, the event comes */
    "MSG_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT",--,                /*!< When scan parameters set complete, the event comes */
    "MSG_GAP_BLE_SCAN_RESULT_EVT",--,                            /*!< When one scan result ready, the event comes each time */
    "MSG_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT",--,              /*!< When raw advertising data set complete, the event comes */
    "MSG_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT",--,         /*!< When raw advertising data set complete, the event comes */
    "MSG_GAP_BLE_ADV_START_COMPLETE_EVT",--,                     /*!< When start advertising complete, the event comes */
    "MSG_GAP_BLE_SCAN_START_COMPLETE_EVT",--,                    /*!< When start scan complete, the event comes */
    "MSG_GAP_BLE_AUTH_CMPL_EVT",-- = 8,                          /* Authentication complete indication. */
    "MSG_GAP_BLE_KEY_EVT",--,                                    /* BLE  key event for peer device keys */
    "MSG_GAP_BLE_SEC_REQ_EVT",--,                                /* BLE  security request */
    "MSG_GAP_BLE_PASSKEY_NOTIF_EVT",--,                          /* passkey notification event */
    "MSG_GAP_BLE_PASSKEY_REQ_EVT",--,                            /* passkey request event */
    "MSG_GAP_BLE_OOB_REQ_EVT",--,                                /* OOB request event */
    "MSG_GAP_BLE_LOCAL_IR_EVT",--,                               /* BLE local IR event */
    "MSG_GAP_BLE_LOCAL_ER_EVT",--,                               /* BLE local ER event */
    "MSG_GAP_BLE_NC_REQ_EVT",--,                                 /* Numeric Comparison request event */
    "MSG_GAP_BLE_ADV_STOP_COMPLETE_EVT",--,                      /*!< When stop adv complete, the event comes */
    "MSG_GAP_BLE_SCAN_STOP_COMPLETE_EVT",--,                     /*!< When stop scan complete, the event comes */
    "MSG_GAP_BLE_SET_STATIC_RAND_ADDR_EVT",-- = 19,              /*!< When set the static rand address complete, the event comes */
    "MSG_GAP_BLE_UPDATE_CONN_PARAMS_EVT",--,                     /*!< When update connection parameters complete, the event comes */
    "MSG_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT",--,                /*!< When set pkt length complete, the event comes */
    "MSG_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT",--,             /*!< When  Enable/disable privacy on the local device complete, the event comes */
    "MSG_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT",--,               /*!< When remove the bond device complete, the event comes */
    "MSG_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT",--,                /*!< When clear the bond device clear complete, the event comes */
    "MSG_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT",--,                  /*!< When get the bond device list complete, the event comes */
    "MSG_GAP_BLE_READ_RSSI_COMPLETE_EVT",--,                     /*!< When read the rssi complete, the event comes */
    "MSG_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT",--,              /*!< When add or remove whitelist complete, the event comes */
    "MSG_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT",--,  /*!< When update duplicate exceptional list complete, the event comes */
    "MSG_GAP_BLE_SET_CHANNELS_EVT",-- = 29,                           /*!< When setting BLE channels complete, the event comes */
    "MSG_GAP_BLE_READ_PHY_COMPLETE_EVT",--,
    "MSG_GAP_BLE_SET_PREFERED_DEFAULT_PHY_COMPLETE_EVT",--,
    "MSG_GAP_BLE_SET_PREFERED_PHY_COMPLETE_EVT",--,
    "MSG_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT",--,
    "MSG_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT",--,34
    "MSG_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT",--,35
    "MSG_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT",--,
    "MSG_GAP_BLE_EXT_ADV_START_COMPLETE_EVT",--,37
    "MSG_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT",--,
    "MSG_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT",--,
    "MSG_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT",--,
    "MSG_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT",--,
    "MSG_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT",--,
    "MSG_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT",--,
    "MSG_GAP_BLE_PHY_UPDATE_COMPLETE_EVT",--,
    "MSG_GAP_BLE_EXT_ADV_REPORT_EVT",--,
    "MSG_GAP_BLE_SCAN_TIMEOUT_EVT",--,
    "MSG_GAP_BLE_ADV_TERMINATED_EVT",--,
    "MSG_GAP_BLE_SCAN_REQ_RECEIVED_EVT",--,
    "MSG_GAP_BLE_CHANNEL_SELETE_ALGORITHM_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_REPORT_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT",--,
    "MSG_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT",--,
    "MSG_GAP_BLE_EVT",--_MAX,
};

local ret = 0

local adv_data_raw0 = { 0x02, 0x01, 0x06, 0x02, 0x0a, 0xeb, 0x03, 0x03, 0xab, 0xcd, 0x11, 0X09, 0x45,0x53,0x50,0x5F,0x42,0x4C,0x45,0x35,0x30,0x5F,0x53,0x45,0x52,0x30,0x30,0x30}
local rand_addr0 = {0xc0, 0xde, 0x52, 0x00, 0x00, 0x00}

local adv_data_raw1 = { 0x02, 0x01, 0x06, 0x02, 0x0a, 0xeb, 0x03, 0x03, 0xab, 0xcd, 0x11, 0X09, 0x45,0x53,0x50,0x5F,0x42,0x4C,0x45,0x35,0x30,0x5F,0x53,0x45,0x52,0x30,0x30,0x31}

local rand_addr1 = {0xc0, 0xde, 0x52, 0x00, 0x00, 0x01}

local serviceHandle
local attrCharHandle
local attrCharDescrHandle

local function adv_config()
    --设置广播0发布参数,时间间隔
	ret = ble.gapExtAdvSetParams(0, 0x30, 0x30)
	log.info("ble func gap_ext_adv_set_params:", ret)

	--设置广播0设备随机地址
	ret = ble.gapExtAdvSetRandAddr(0, rand_addr0);
	log.info("ble func gap_ext_adv_set_rand_addr:", ret)
	
	--设置广播0信息
	ret = ble.gapConfigExtAdvDataRaw(0, adv_data_raw0)
	log.info("ble func gap_config_ext_adv_data_raw:", ret)

	--设置广播1发布参数,时间间隔
	ret = ble.gapExtAdvSetParams(1, 0x40, 0x40)
	log.info("ble func gap_ext_adv_set_params:", ret)

	--设置广播1设备随机地址
	ret = ble.gapExtAdvSetRandAddr(1, rand_addr1);
	log.info("ble func gap_ext_adv_set_rand_addr:", ret)
	
	--设置广播1信息
	ret = ble.gapConfigExtAdvDataRaw(1, adv_data_raw1)
	log.info("ble func gap_config_ext_adv_data_raw:", ret)
	
	--打开2个广播
	ret = ble.gapExtAdvStart(2)
	log.info("ble func gap_ext_adv_star:", ret)
end

local function ble_cb( ... ) 
	local event = select(1, ...)
	local status = select(2, ...)
    local type;
	if event < 0x20000 then
		--GAP MSG
        type = 1
		log.info("ble event id", event-0x10000, " ble.", msg_gap[event-0x10000+1])
	else
		--GATTS MSG
        type = 2
		log.info("ble event id", event-0x20000, " ble.", msg_gatts[event-0x20000+1])
	end

	--GAP MSG
    if type == 1 then 
        --gap 消息至少两个参数
        --arg1 event:消息id
        --arg2 status:状态0正常，其他失败
        
    elseif type == 2 then 
        --gatss 消息至少三个参数
        --arg1 event:消息id
        --arg2 status:状态0正常，其他失败
        --arg3 gattsIf:app标识
        for i,j in pairs({...})do
            log.info("ble event arg ", i .. " j:", j)
        end

        local gattsIf = select(3, ...)

        if ble.MSG_GATTS_REG_EVT == event then
            --app注册结果判断
            if status == 0 then
                local uuid = {0xfe, 0x00}
                --app注册成功
                --创建服务
                --arg1 app标识
                --arg2 服务uuid
                --arg3 属性的个数包括特征和描述
                ret = ble.gattsCreateService(gattsIf, uuid, 4)
                log.info("ble func gatts_create_server:", ret)
            end
        
        elseif ble.MSG_GATTS_CONNECT_EVT == event then
            --链接成功后会自动关闭广播
        elseif ble.MSG_GATTS_DISCONNECT_EVT == event then
            --断开后需要重新打开广播才能再次链接
            --打开2个广播
            ret = ble.gapExtAdvStart(2)
            log.info("ble func gap_ext_adv_star:", ret)
        elseif ble.MSG_GATTS_CREATE_EVT == event then
            --判断服务创建状态
            if status == 0 then
                --服务创建成功
                --获取服务句柄
                serviceHandle = select(4, ...)
                --开始服务
                --arg1 服务句柄
                ret = ble.gattsStartService(serviceHandle)
                log.info("ble func gatts_start_service:", ret)

                --添加特征
                --arg1 服务句柄
                --arg2 特征uuid
                --arg3 特征数据
                local attrCharUuid = {0xff, 0x00}
                local attrCharVal = {1,2,3,4,5}
                ret = ble.gattsAddChar(serviceHandle, attrCharUuid, attrCharVal)
                log.info("ble func gatts_add_char:", ret)

            end
        elseif ble.MSG_GATTS_START_EVT == event then
            --服务启动结果判断
            if status == 0 then
                --服务启动成功
                
            end
        elseif ble.MSG_GATTS_ADD_CHAR_EVT == event then
            --添加特征判断
            if status == 0 then
                --特征添加成功
                --获取特征句柄
                attrCharHandle = select(4, ...)
                --添加描述
                local attrCharDescrUuid = {0xff, 0x02}
                local attrCharDescrVal = {6,7,8,9,10}
                ret = ble.gattsAddCharDescr(serviceHandle, attrCharDescrUuid, attrCharDescrVal)
                log.info("ble func gatts_add_char_descr:", ret)
            end
        elseif ble.MSG_GATTS_ADD_CHAR_DESCR_EVT == event then
            --添加特征判断
            if status == 0 then
                --特征添加成功
                --获取描述句柄
                attrCharDescrHandle = select(4, ...)
                
                adv_config()
            end
        
        elseif ble.MSG_GATTS_READ_EVT == event then
            --从设备读数据,不需要判断status
            local attrHandle = select(4, ...)
            local connId = select(5, ...)
            local transId = select(6, ...)
            local rsp = {0x32,0x30,0x32,0x32,0x72 ,0x65 ,0x61 ,0x64 ,0x20 ,0x74 ,0x65 ,0x73 ,0x74} --2022 read test
            
            ret = ble.gattsSendResponse(gattsIf, attrHandle, connId, transId, rsp)
            log.info("ble func gatts_send_response:", ret)
            
        elseif ble.MSG_GATTS_WRITE_EVT == event then
            --从设备写数据,不需要判断status
            local attrHandle = select(4, ...)
            local connId = select(5, ...)
            local transId = select(6, ...)
            local rsp = {0}
	 
             --给一个应答
            ble.gattsSendResponse(gattsIf, attrHandle, connId, transId, rsp)

            local  ret, handle, connId, transId, data = ble.gattsRead()
            if ret == 0 then
                log.info("ble func gatts_read: handle ", handle)
                log.info("ble func gatts_read: connId ", connId)
                log.info("ble func gatts_read: transId ", transId)
                log.info("ble func gatts_read: dataLen ", #data)
                log.info("ble func gatts_read: data ", string.toHex(data))
            else
                log.info("ble func gatts_read: error", ret)
            end
            
        end
    end
end



sys.taskInit(function ( ... )
	--初始化蓝牙
    ret = ble.init(ble_cb)
    log.info("ble func init:", ret)
	
    ret = ble.gattsAppRegist(0)
    log.info("ble func gatts_app_regist:", ret)
end)

sys.run()

