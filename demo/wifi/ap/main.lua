PROJECT = "wifi-ap-demo"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"

sys.taskInit(
    function()
        local w = wlan
        log.info("wlan", "wlan_init:", wlan.init())

        wlan.setMode(wlan.AP)
        log.info("wlan.createAP",wlan.createAP("c3-luatos","12345678"))
        log.info("wlan.dhcp",wlan.dhcp(0)) -- 关闭dhcp
        log.info("wlan.setip",wlan.setIp("192.168.55.1", "192.168.55.1", "255.255.255.0"))
        log.info("wlan.dhcp",wlan.dhcp(1)) -- 开启dhcp

    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
