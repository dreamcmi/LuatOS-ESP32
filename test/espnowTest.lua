local espnowTest = {}

local tag = "espnowTest"

function espnowTest.test()
    if espnow == nil then
        log.error(tag, "this fireware is not support espnow")
        return
    end
    log.info(tag, "START")
    sys.taskInit(function()
        log.info("espnow init", espnow.init())
        log.info("espnow setpmk", espnow.setPmk("pmk1234567890123"))
        log.info("espnow add", espnow.addPeer(string.fromHex("FFFFFFFFFFFF"),
                                              "lmk1234567890123"))
        while 1 do
            espnow.send(string.fromHex("FFFFFFFFFFFF"), "lua-espnow")
            sys.wait(1000)
        end
    end)

    sys.subscribe("ESPNOW_RECV", function(mac, data, len)
        log.info("ESPNOW_RECV", string.toHex(mac))
        log.info("ESPNOW_RECV", data)
        log.info("ESPNOW_RECV", len)
    end)

    sys.subscribe("ESPNOW_SEND", function(mac, status)
        log.info("ESPNOW_SEND", string.toHex(mac))
        log.info("ESPNOW_SEND", status)
    end)
    log.info(tag, "DONE")
end

return espnowTest
