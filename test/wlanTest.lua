wlanTest = {}

local tag = "wlanTest"

function wlanTest.test()
    if wlan == nil then
        log.error(tag, "this fireware is not support wlan")
        return
    end
    log.info(tag, "START")
    assert(wlan.init() == 0, tag .. ".init ERROR")
    assert(wlan.setMode(wlan.STATION) == 0, tag .. ".setMode ERROR")
    assert(wlan.connect("Xiaomi_AX6000", "Air123456") == 0,
           tag .. ".connect ERROR")
    assert(wlan.disconnect() == 0, tag .. ".disconnect ERROR")
    assert(wlan.deinit() == 0, tag .. ".deinit ERROR")
    log.info(tag, "DONE")

end

return wlanTest
