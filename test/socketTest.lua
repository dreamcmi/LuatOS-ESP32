socketTest = {}

local tag = "socketTest"

function socketTest.test()
    if socket == nil then
        log.error(tag, "this fireware is not support socket")
        return
    end
    log.info(tag, "START")
    sys.taskInit(function()
        ret = wlan.init()
        log.info("wlan", "wlan_init:", ret)
        wlan.setMode(wlan.STATION)
        werr = wlan.connect("Xiaomi_AX6000", "Air123456")
        log.info("wlan", "wait connect", werr)

        sys.wait(2 * 1000) -- 稍微延时下

        log.info("socket", "begin socket")
        local sock = socket.creat(socket.TCP) -- tcp

        repeat
            err = socket.connect(sock, "112.125.89.8", 37674)
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
    end)
    log.info(tag, "DONE")
end

return socketTest
