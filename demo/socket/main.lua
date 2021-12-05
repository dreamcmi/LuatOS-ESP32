PROJECT = "socket"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"

sys.taskInit(
    function()
        ret = wlan.init()
        log.info("wlan", "wlan_init:", ret)
        wlan.setMode(wlan.STATION)
        werr = wlan.connect("lua123456", "lua123456")
        log.info("wlan", "wait connect",werr)
        
        sys.wait(2 * 1000) -- 稍微延时下

        log.info("socket", "begin socket")
        local sock = socket.creat(socket.TCP) -- tcp

        repeat
            err = socket.connect(sock, "112.125.89.8", 37300)
            print("socket connect wait")
            sys.wait(1000)
        until (err == 0)
        log.info("socket", "connected")

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
