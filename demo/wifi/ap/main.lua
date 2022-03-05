PROJECT = "wifi-ap-demo"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"
local AP_MODE = 1

sys.taskInit(
    function()
        local w = wlan
        log.info("wlan", "wlan_init:", wlan.init())

        wlan.setMode(wlan.AP)
        log.info("wlan.createAP", wlan.createAP("c3-luatos", "12345678"))
        log.info("wlan.dhcp", wlan.dhcp(0)) -- 关闭dhcp
        log.info("wlan.setip", wlan.setIp("192.168.55.1", "192.168.55.1", "255.255.255.0"))
        log.info("wlan.dhcp", wlan.dhcp(1)) -- 开启dhcp

        t = wlan.getConfig(AP_MODE)
        log.info("wlan", "wifi ap info", t.ssid, t.password)

        -- 调用下面一行关闭wifi
        -- log.info("wlan", "stop", wlan.stop())
    end
)

sys.subscribe(
    "WLAN_AP_START",
    function()
        log.info("wlan", "WLAN_AP_START")
    end
)

sys.subscribe(
    "WLAN_AP_STACONNECTED",
    function(mac, aid)
        log.info("wlan", "WLAN_AP_STACONNECTED", aid, mac:toHex())
    end
)

sys.subscribe(
    "WLAN_AP_STADISCONNECTED",
    function(mac, aid)
        log.info("wlan", "WLAN_AP_STADISCONNECTED", aid, mac:toHex())
    end
)

sys.subscribe(
    "WLAN_AP_STOP",
    function()
        log.info("wlan", "WLAN_AP_STOP")
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
