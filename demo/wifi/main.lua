PROJECT = "wifidemo"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"

sys.taskInit(
    function()
        log.info("wlan", "wlan_init:", wlan.init())

        sys.waitUntil("WLAN_READY", 1000)
        log.info("wlan", "WLAN_READY")

        wlan.setMode(wlan.STATION)
        wlan.connect("xxxx", "123456789")
        result, _ = sys.waitUntil("WLAN_STA_CONNECTED", 3000)
        log.info("wlan", "WLAN_STA_CONNECTED", result)

        sys.wait(5000)

        wlan.disconnect()
        result, _ = sys.waitUntil("WLAN_STA_DISCONNECTED", 3000)
        log.info("wlan", "WLAN_STA_DISCONNECTED", result)

        log.info("wlan", "wlan_deinit", wlan.deinit())
    end
)
-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
