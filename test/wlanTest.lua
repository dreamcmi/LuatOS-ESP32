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
    assert(wlan.getMode() == wlan.STATION, tag .. ".getMode ERROR")
    assert(wlan.setps(wlan.PS_MAX_MODEM) == 0, tag .. ".setps ERROR")
    assert(wlan.getps() == wlan.PS_MAX_MODEM, tag .. ".getps ERROR")
    assert(wlan.setps(wlan.PS_MIN_MODEM) == 0, tag .. ".setps ERROR")
    assert(wlan.getps() == wlan.PS_MIN_MODEM, tag .. ".getps ERROR")
    assert(wlan.setps(wlan.PS_NONE) == 0, tag .. ".setps ERROR")
    assert(wlan.getps() == wlan.PS_NONE, tag .. ".getps ERROR")
    assert(wlan.setBandwidth(wlan.IF_STA, wlan.HT20) == 0,
           tag .. ".setBandwidth ERROR")
    assert(wlan.getBandwidth(wlan.IF_STA) == wlan.HT20,
           tag .. ".getBandwidth ERROR")
    assert(wlan.setBandwidth(wlan.IF_STA, wlan.HT40) == 0,
           tag .. ".setBandwidth ERROR")
    assert(wlan.getBandwidth(wlan.IF_STA) == wlan.HT40,
           tag .. ".getBandwidth ERROR")
    assert(wlan.setHostname("LuatOS-ESP32C3-" .. tag) == 0,
           tag .. ".setHostname ERROR")
    assert(wlan.setProtocol(wlan.IF_STA, wlan.P11B) == 0,
           tag .. ".setProtocol ERROR")
    assert(wlan.getProtocol(wlan.IF_STA) == wlan.P11B,
           tag .. ".getProtocol ERROR")
    assert(wlan.setProtocol(wlan.IF_STA, wlan.P11B | wlan.P11G) == 0,
           tag .. ".setProtocol ERROR")
    assert(wlan.getProtocol(wlan.IF_STA) == wlan.P11B | wlan.P11G,
           tag .. ".getProtocol ERROR")
    assert(
        wlan.setProtocol(wlan.IF_STA, wlan.P11B | wlan.P11G | wlan.P11N) == 0,
        tag .. ".setProtocol ERROR")
    assert(wlan.getProtocol(wlan.IF_STA) == wlan.P11B | wlan.P11G | wlan.P11N,
           tag .. ".getProtocol ERROR")
    --     assert(wlan.setProtocol(wlan.IF_STA,
    --                             wlan.P11B | wlan.P11G | wlan.P11N | wlan.LR) == 0,
    --            tag .. ".setProtocol ERROR")
    --     assert(wlan.getProtocol(wlan.IF_STA) == wlan.P11B | wlan.P11G | wlan.P11N |
    --                wlan.LR, tag .. ".getProtocol ERROR")
    local WLAN_SSID = "Xiaomi_AX6000"
    local WLAN_PASSWD = "Air123456"
    assert(wlan.connect(WLAN_SSID, WLAN_PASSWD) == 0, tag .. ".connect ERROR")
    waitRes, data = sys.waitUntil("WLAN_STA_CONNECTED", 10000)
    log.info(tag .. ".WLAN_STA_CONNECTED", waitRes, data)
    if waitRes == false then return end
    waitRes, data = sys.waitUntil("IP_READY", 10000)
    log.info(tag .. ".IP_READY", waitRes, data)
    if waitRes == false then return end
    local wlanInfo = wlan.getConfig(0)
    assert(wlanInfo.ssid == WLAN_SSID and wlanInfo.password == WLAN_PASSWD,
           tag .. ".getConfig ERROR")
    log.info(tag .. ".bssid", wlanInfo.bssid:toHex())

    assert(wlan.disconnect() == 0, tag .. ".disconnect ERROR")
    waitRes, data = sys.waitUntil("WLAN_STA_DISCONNECTED", 10000)
    log.info(tag .. ".WLAN_STA_DISCONNECTED", waitRes, data)
    if waitRes == false then return end

    log.info("smartconfig", "start")
    local r = wlan.taskSmartconfig(0, 300).wait()
    log.info("smartconfig", r and "connected!" or "connect fail")

    result, _ = sys.waitUntil("SMARTCONFIG_ACK_DONE")
    log.info("wlan", "SMARTCONFIG_ACK_DONE", result)

    t = wlan.getConfig(0)
    log.info("wlan", "wifi connected info", t.ssid, t.password, t.bssid:toHex())

    assert(wlan.disconnect() == 0, tag .. ".disconnect ERROR")
    waitRes, data = sys.waitUntil("WLAN_STA_DISCONNECTED", 10000)
    log.info(tag, "WLAN_STA_DISCONNECTED", waitRes, data)

    if wlan.smartconfig() ~= 0 then
        log.error(tag .. ".connect", "ERROR")
        return false
    end
    waitRes, data = sys.waitUntil("WLAN_STA_CONNECTED", 180 * 10000)
    log.info("WLAN_STA_CONNECTED", waitRes, data)
    if waitRes ~= true then
        log.error(tag .. ".wlan ERROR")
        return false
    end
    -- log.info("smartconfigStop", wlan.smartconfigStop())
    waitRes, data = sys.waitUntil("IP_READY", 10000)
    if waitRes ~= true then
        log.error(tag .. ".wlan ERROR")
        return false
    end
    log.info("IP_READY", waitRes, data)

    local AP_SSID = "LuatOS-ESP32-APTest"
    local AP_PASSWD = "12345678"
    assert(wlan.setMode(wlan.AP) == 0, tag .. ".setMode ERROR")
    assert(wlan.getMode() == wlan.AP, tag .. ".getMode ERROR")
    assert(wlan.dhcp(0) == 0, tag .. ".dhcp ERROR")
    assert(wlan.setIp("192.168.55.1", "192.168.55.1", "255.255.255.0") == 0,
           tag .. ".setIp ERROR")
    assert(wlan.dhcp(1) == 0, tag .. ".dhcp ERROR")
    assert(wlan.createAP(AP_SSID, AP_PASSWD) == 0, tag .. ".createAP ERROR")
    local apInfo = wlan.getConfig(1)
    assert(apInfo.ssid == AP_SSID and apInfo.password == AP_PASSWD,
           tag .. ".getConfig ERROR")
    log.info(tag .. ".AP", "AP开启中,一分钟后关闭")
    sys.wait(1000 * 60)
    assert(wlan.stop() == 0, tag .. ".stop ERROR")
    assert(wlan.deinit() == 0, tag .. ".deinit ERROR")
    log.info(tag, "DONE")
end

return wlanTest
