PROJECT = "socket-udp-client"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

sys.taskInit(
    function()
        ret = wlan.init()
        log.info("wlan", "wlan_init:", ret)
        wlan.setMode(wlan.STATION)
        wlan.connect("xxxx", "123456789")
        -- 等到成功获取ip就代表连上局域网了
        result, data = sys.waitUntil("IP_READY")
        log.info("wlan", "IP_READY", result, data)

        log.info("socket", "begin socket")
        local sock = socket.create(socket.UDP) -- udp

        log.info("socket.bind", socket.bind(sock, "0.0.0.0", 8684)) --udp必须绑定端口

        repeat
            err = socket.connect(sock, "112.125.89.8", 36307)
            log.info("socket", err)
            sys.wait(3000)  -- 重试间隔
        until (err == 0)

        len = socket.send(sock, "hello lua esp32c3")
        log.info("socket", "sendlen", len)

        while 1 do
            local data, len = socket.recv(sock)
            if data ~= nil then
                if data == "close" then
                    socket.close(sock)
                    log.info("socket", "close")
                end
                log.info("socket", "len", len)
                log.info("socket", "data", data)
            end
            sys.wait(1000)
        end
        log.info("socket", "end")
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
