PROJECT = "wifidemo"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"

log.info("main", "ask for help", "https://wiki.luatos.com")

sys.taskInit(
    function()
        log.info("wlan", "wlan_init:", wlan.init())

        wlan.setMode(wlan.STATION)
        wlan.connect("uiot", "1234567890")

        -- 参数已配置完成，后台将自动开始连接wifi
        result, _ = sys.waitUntil("WLAN_READY")
        log.info("wlan", "WLAN_READY", result)
        -- 等待连上路由,此时还没获取到ip
        result, _ = sys.waitUntil("WLAN_STA_CONNECTED")
        log.info("wlan", "WLAN_STA_CONNECTED", result)
        -- 等到成功获取ip就代表连上局域网了
        result, data = sys.waitUntil("IP_READY")
        log.info("wlan", "IP_READY", result, data)


        
        local mqtt_topic = "/luatos/esp32c3/mqttc/request" .. esp32.getmac():toHex()

        while true do
            local mqttc = espmqtt.init({
                uri = "mqtt://lbsmqtt.airm2m.com:1884",
                client_id = (esp32.getmac():toHex())
            })
            log.info("mqttc", mqttc)
            if mqttc then
                log.info("mqttc", "what happen")
                local ok, err = espmqtt.start(mqttc)
                log.info("mqttc", "start", ok, err)
                if ok then
                    while 1 do
                        log.info("mqttc", "wait ESPMQTT_EVT 30s")
                        local result, c, ret, topic, data = sys.waitUntil("ESPMQTT_EVT", 30000)
                        log.info("mqttc", result, c, ret)
                        if result then
                            -- 没消息, 没动静
                            log.info("mqttc", "wait timeout")
                            -- 这里演示给自己发一条. 按实际业务来定
                            espmqtt.publish(mqttc, mqtt_topic, "{}")
                        elseif c == mqttc then
                            -- 是当前mqtt客户端的消息, 处理之
                            if ret == espmqtt.EVENT_CONNECTED then
                                -- 连接成功, 通常就是定义一些topic
                                espmqtt.subscribe(mqttc, mqtt_topic)
                            elseif ret == espmqtt.EVENT_DATA then
                                -- 服务器来消息了, 如果data很长(超过4kb), 可能会分多次接收, 导致topic为空字符串
                                log.info("mqttc", topic, data)
                            else
                                -- qos > 0 的订阅信息有响应, 会进这个分支
                                -- 默认情况下mqtt是自动重连的, 不需要用户关心
                                log.info("mqttc", "event", ret)
                            end
                        else
                            log.info("mqttc", "not this mqttc")
                        end
                    end
                else
                    log.warn("mqttc", "bad start", err)
                end
                espmqtt.destroy(mqttc)
            end
            sys.wait(5000)
        end
    end
)
-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
