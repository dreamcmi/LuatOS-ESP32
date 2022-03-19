PROJECT = "espnow"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

sys.taskInit(
    function()
        log.info("espnow init", espnow.init())
        log.info("espnow setpmk", espnow.setPmk("pmk1234567890123"))
        log.info("espnow add", espnow.addPeer(string.fromHex("FFFFFFFFFFFF"), "lmk1234567890123"))
        while 1 do
            espnow.send(string.fromHex("FFFFFFFFFFFF"), "lua-espnow")
            sys.wait(1000)
        end
    end
)

sys.subscribe(
    "ESPNOW_RECV",
    function(mac, data, len)
        log.info("ESPNOW_RECV", string.toHex(mac))
        log.info("ESPNOW_RECV", data)
        log.info("ESPNOW_RECV", len)
    end
)

sys.subscribe(
    "ESPNOW_SEND",
    function(mac, status)
        log.info("ESPNOW_SEND", string.toHex(mac))
        log.info("ESPNOW_SEND", status)
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
