PROJECT = "smartconfigdemo"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"

sys.taskInit(
    function() 
        log.info("wlan", "wlan_init:", wlan.init())
        wlan.setMode(wlan.STATION)

        log.info("smartconfig", wlan.smartconfig())

        result, _ = sys.waitUntil("WLAN_STA_CONNECTED")
        log.info("wlan", "WLAN_STA_CONNECTED RESULT", result)

        -- WIFI连接上之后，还需要等段时间才能关闭smartconfig，不然smartconfig ack有可能无法发出，其实这里不用关闭，底层会自己关闭的
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
