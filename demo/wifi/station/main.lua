PROJECT = "wifi-station-demo"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"

sys.taskInit(
    function()
        local w = wlan
        log.info("wlan", "wlan_init:", wlan.init())

        wlan.setMode(wlan.STATION)
        wlan.connect("xxxx", "123456789")   --此函数第三个参数为1时开启自动重连
        -- 参数已配置完成，后台将自动开始连接wifi
        result, _ = sys.waitUntil("WLAN_READY")
        log.info("wlan", "WLAN_READY", result)
        -- 等待连上路由,此时还没获取到ip
        result, _ = sys.waitUntil("WLAN_STA_CONNECTED")
        log.info("wlan", "WLAN_STA_CONNECTED", result)
        -- 等到成功获取ip就代表连上局域网了
        result, data = sys.waitUntil("IP_READY")
        log.info("wlan", "IP_READY", result, data)

        sys.wait(10*1000)
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
