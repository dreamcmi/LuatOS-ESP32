local wlanTest = {}

local tag = "wlanTest"

function wlanTest.test()
    if wlan == nil then
        log.error(tag, "this fireware is not support wlan")
        return
    end
    local waitRes, data
    log.info(tag, "START")
    assert(wlan.init() == 0, tag .. ".init ERROR")
    assert(wlan.setMode(wlan.STATION) == 0, tag .. ".setMode ERROR")
    assert(wlan.connect("Xiaomi_AX6000", "Air123456") == 0,
           tag .. ".connect ERROR")
    waitRes, data = sys.waitUntil("WLAN_READY", 10000)
    log.info(tag, "WLAN_READY", waitRes, data)
    waitRes, data = sys.waitUntil("WLAN_STA_CONNECTED", 10000)
    log.info(tag, "WLAN_STA_CONNECTED", waitRes, data)

    waitRes, data = sys.waitUntil("IP_READY", 10000)
    log.info(tag, "IP_READY", waitRes, data)
    assert(wlan.disconnect() == 0, tag .. ".disconnect ERROR")
    waitRes, data = sys.waitUntil("WLAN_STA_DISCONNECTED", 10000)
    log.info(tag, "WLAN_STA_DISCONNECTED", waitRes, data)
    assert(wlan.deinit() == 0, tag .. ".deinit ERROR")
    log.info(tag, "DONE")
end

return wlanTest
