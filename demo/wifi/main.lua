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
        -- 等待连上路由,此时还没获取到ip
        result, _ = sys.waitUntil("WLAN_STA_CONNECTED")
        log.info("wlan", "WLAN_STA_CONNECTED", result)
        -- 等到成功获取ip就代表连上局域网了
        result, data = sys.waitUntil("IP_READY")
        log.info("wlan", "IP_READY", result, data)

        wlan.disconnect()
        result, _ = sys.waitUntil("WLAN_STA_DISCONNECTED")
        log.info("wlan", "WLAN_STA_DISCONNECTED", result)

        log.info("wlan", "wlan_deinit", wlan.deinit())
    end
)
-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
