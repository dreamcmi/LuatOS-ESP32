PROJECT = "socket-server"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

sys.taskInit(
    function()
        log.info("wlan", "wlan_init:", wlan.init())
        wlan.setMode(wlan.STATION)
        wlan.connect("xxxx", "123456789")
        -- 等到成功获取ip就代表连上局域网了
        result, data = sys.waitUntil("IP_READY")
        log.info("wlan", "IP_READY", result, data)

        log.info("socket", "begin socket")
        local sock = socket.create(socket.UDP) -- tcp

        log.info("socket.bind", socket.bind(sock, "0.0.0.0", 8684))

        while 1 do
            local data, len = socket.recv(sock)
            if data ~= nil then
                log.info("socket.client", "data", data)
                log.info("socket.client", "len", len)
                -- socket.send(sock, "hello lua esp32c3")
            end
            sys.wait(1000)
        end
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
