local esphttpTest = {}

local tag = "esphttpTest"
local serverAddr = "47.96.229.157:2900"
local waitRes, data

function esphttpTest.test()
    if esphttp == nil then
        log.error(tag, "this fireware is not support esphttp")
        return
    end
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

    local httpClient = esphttp.init(esphttp.GET,
                                    serverAddr ..
                                        "/?test1=1&test2=22&test3=333&test4=" ..
                                        string.urlEncode("四四四四") ..
                                        "&test5=FiveFiveFiveFiveFive&test6=" ..
                                        string.urlEncode(
                                            "ろくろくろくろくろくろく"))
    if httpClient then
        local ok, err = esphttp.perform(httpClient, true)
        if ok then
            while 1 do
                local result, c, ret, data = sys.waitUntil("ESPHTTP_EVT", 20000)
                log.info("httpc", result, c, ret)
                if c == httpClient then
                    if esphttp.is_done(httpClient, ret) then
                        break
                    end
                    if ret == esphttp.EVENT_ON_DATA and
                        esphttp.status_code(httpClient) == 200 then
                        log.info("data", "resp", data)
                    end
                end
            end
        else
            log.warn("esphttp", "bad perform", err)
        end
        esphttp.cleanup(httpClient)
    end
    assert(wlan.disconnect() == 0, tag .. ".disconnect ERROR")
    waitRes, data = sys.waitUntil("WLAN_STA_DISCONNECTED", 10000)
    log.info(tag, "WLAN_STA_DISCONNECTED", waitRes, data)
    assert(wlan.deinit() == 0, tag .. ".deinit ERROR")
    log.info(tag, "DONE")

end

return esphttpTest
