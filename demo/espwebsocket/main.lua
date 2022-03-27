PROJECT = "espwebsocket"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"

sys.taskInit(
    function()
        log.info("wlan", "wlan_init:", wlan.init())

        wlan.setMode(wlan.STATION)
        wlan.connect("xxxx", "12345678")

        -- 等到成功获取ip就代表连上局域网了
        result, data = sys.waitUntil("IP_READY")
        log.info("wlan", "IP_READY", result, data)

        local ws = espws.init("ws://airtest.openluat.com:2900/websocket")

        if ws then
            local count = 0
            log.info("ws.start", espws.start(ws))
            sys.waitUntil("WS_CONNECT")
            log.info("ws", "connected")
            while count < 10 do
                log.info("ws.send" .. count, espws.send(ws, "hello luatos esp32"))
                local _, client, data, len = sys.waitUntil("WS_EVENT_DATA")
                if ws == client then
                    log.info("ws.recv" .. count, "client is ws")
                    log.info("ws.recv" .. count, data, len)
                else
                    log.warn("ws", "client is not ws")
                end
                count = count + 1
                sys.wait(1000)
            end
        else
            log.info("ws.client", "ws.clint init error!!!")
        end

        log.info("ws.close", espws.close(ws))
        log.info("ws.destory", espws.destory(ws))
    end
)
-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
