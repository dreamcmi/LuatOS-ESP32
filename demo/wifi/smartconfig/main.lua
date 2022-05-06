PROJECT = "smartconfigdemo"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"
local STA_MODE = 0

sys.taskInit(
    function() 
        log.info("wlan", "wlan_init:", wlan.init())
        wlan.setMode(wlan.STATION)

        log.info("smartconfig", wlan.smartconfig())

        result, _ = sys.waitUntil("WLAN_STA_CONNECTED")
        log.info("wlan", "WLAN_STA_CONNECTED RESULT", result)

        t = wlan.getConfig(STA_MODE)
        log.info("wlan", "wifi connected info", t.ssid, t.password, t.bssid:toHex())
    end
)

-- sys.taskInit(
--     function()
--         log.info("wlan", "wlan_init:", wlan.init())
--         wlan.setMode(wlan.STATION)

--         log.info("smartconfig","start")
--         local r = wlan.taskSmartconfig(0,120).wait() --模式0，超时120秒
--         log.info("smartconfig", r and "connected!" or "connect fail")

--         -- ack代表返回手机配网成功,根据自身情况选择是否开启
--         result, _ = sys.waitUntil("SMARTCONFIG_ACK_DONE")
--         log.info("wlan", "SMARTCONFIG_ACK_DONE", result)

--         t = wlan.getConfig(STA_MODE)
--         log.info("wlan", "wifi connected info", t.ssid, t.password, t.bssid:toHex())
--     end
-- )

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
