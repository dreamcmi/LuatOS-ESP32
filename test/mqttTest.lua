local mqttTest = {}

local tag = "mqttTest"

function mqttTest.test()
    if lwip == nil then
        log.error(tag, "this fireware is not support lwip")
        return
    end
    sys.taskInit(function()
        log.info("WIFI", "START")
        assert(wlan.init() == 0, tag .. ".init ERROR")
        assert(wlan.setMode(wlan.STATION) == 0, tag .. ".setMode ERROR")
        assert(wlan.connect("Xiaomi_AX6000", "Air123456") == 0,
               tag .. ".connect ERROR")
        result, _ = sys.waitUntil("WLAN_STA_CONNECTED")
        log.info("wlan", "WLAN_STA_CONNECTED", result)
        log.info(tag, "START")
        local mqttc = lwip.mqtt_new()
        assert(mqttc ~= nil, tag .. ".mqtt_new ERROR")
        lwip.mqtt_arg(mqttc, {
            conn = function(status)
                if status == lwip.CONNECTED then
                    lwip.subscribe(mqttc, "/LuatOSSubscribeTest", 0)
                    lwip.subscribe(mqttc, "/LuatOS订阅测试", 1)
                else
                    log.info(tag, "MQTT_CLOSED")
                end
            end,
            inpub = function(topic, data)
                log.info("mqtt", topic, data)
            end,
            req = function(result) log.info("REQ", result) end
        })
        while 1 do
            local con_res = lwip.mqtt_connect(mqttc, "47.96.229.157", 1883, {
                client_id = "TESTTESTTESTTESTTEST",
                user = "test",
                pass = "test",
                keep_alive = 300
            })
            log.info("CON_RES", con_res)
            if con_res then
                print(lwip.mqtt_is_connected(mqttc))
                while lwip.mqtt_is_connected(mqttc) do
                    sys.waitUtil("MQTT_CLOSED", 2000)
                end
            end
            sys.wait(5000)
        end
        log.info(tag, "DONE")
    end)
end

return mqttTest
