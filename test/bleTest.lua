bleTest = {}

local tag = "bleTest"

function bleTest.test()
    if ble == nil then
        log.error(tag, "this fireware is not support ble")
        return
    end
    log.info(tag, "START")
    ret = ble.init()
    log.info("ble", "ble_init:", ret)
    ret2 = ble.deinit()
    log.info("ble", "ble_deinit", ret2)
    log.info(tag, "DONE")
end

return bleTest
