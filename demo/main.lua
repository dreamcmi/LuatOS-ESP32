
-- LuaTools需要PROJECT和VERSION这两个信息
PROJECT = "demo"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"
sys.subscribe("WLAN_READY", function ()
    print("!!! wlan ready event !!!")
end)
sys.subscribe("WLAN_STA_DISCONNECTED", function ()
    print("!!! WLAN_STA_DISCONNECTED event !!!")
end)

sys.taskInit(function()
    gpio.setup(18,1)
    log.info("18", "start")
    wlan.init()
    wlan.setMode(wlan.STATION)
    wlan.connect("MT7628", "1234567890")
    while 1 do
        log.info("wifimode",wlan.getMode())     
        gpio.set(18,0)
        log.info("18", "0")
        sys.wait(1000)   
        gpio.set(18,1)
        log.info("18", "1")
        sys.wait(1000)
        wlan.disconnect()
    end
end)


-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
