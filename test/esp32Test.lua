local esp32Test = {}

local tag = "esp32Test"

function esp32Test.test()
    if esp32 == nil then
        log.error(tag, "this fireware is not support esp32")
        return
    end
    log.info(tag, "START")
    log.info("esp32", "rstReason", esp32.getRstReason())
    local re = esp32.getchip()
    log.info("esp32", "chip", re["chip"])
    log.info("esp32", "features", re["features"])
    log.info("esp32", "cores", re["cores"])
    log.info("esp32", "revision", re["revision"])

    stamac = esp32.getmac(0)
    log.info("stamac", string.toHex(stamac))

    apmac = esp32.getmac(1)
    log.info("apmac", string.toHex(apmac))

    btmac = esp32.getmac(2)
    log.info("btmac", string.toHex(btmac))

    ethmac = esp32.getmac(3)
    log.info("ethmac", string.toHex(ethmac))

    log.info("esp32", "random", esp32.random())

    log.info(tag, "DONE")
end

return esp32Test
