local sfudTest = {}

local tag = "sfudTest"

function sfudTest.test()
    if sfud == nil then
        log.error(tag, "this fireware is not support sfud")
        return
    end
    log.info(tag, "START")
    local spiFlash
    if MOD_TYPE == "air101" then
        spiFlash = spi.deviceSetup(0, 17, 0, 0, 8, 2 * 1000 * 1000, spi.MSB, 1,
                                   1)
    elseif MOD_TYPE == "ESP32C3" then
        spiFlash = spi.deviceSetup(2, 6, 0, 0, 8, 60 * 1000 * 1000, spi.MSB, 1,
                                   1)
    end
    assert(sfud.init(spiFlash) == true, tag .. ".init ERROR")
    assert(sfud.getDeviceNum() == 1, tag .. ".getDeviceNum ERROR")
    local sfudDevice = sfud.getDeviceTable()
    assert(sfud.eraseWrite(sfudDevice, 0, tag) == 0, tag .. ".eraseWrite ERROR")
    -- assert(sfud.write(sfudDevice, 0, tag) == 0, tag .. ".write ERROR")
    assert(sfud.read(sfudDevice, 0, 8) == tag, tag .. ".read ERROR")
    assert(sfud.mount(sfudDevice, "/sfud") == true, tag .. ".mount ERROR")
    log.info(tag .. ".fsstat", fs.fsstat("/sfud"))
    log.info(tag, "DONE")
end

return sfudTest
