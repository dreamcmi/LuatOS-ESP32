PROJECT = "wifidemo"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"
local testData = string.rep("socketTest", 100)
local tag = "socketTest"

sys.taskInit(
    function()
        log.info("wlan", "wlan_init:", wlan.init())
        wlan.setMode(wlan.STATION)

        log.info("smartconfig", wlan.smartconfig())

        result, _ = sys.waitUntil("WLAN_STA_CONNECTED")
        log.info("wlan", "WLAN_STA_CONNECTED", result)
        -- 连上之后关闭smartconfig
        log.info("smartconfig",wlan.smartconfigStop())

    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
