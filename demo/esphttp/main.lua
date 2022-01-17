PROJECT = "wifidemo"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"

sys.taskInit(
    function()
        log.info("wlan", "wlan_init:", wlan.init())

        wlan.setMode(wlan.STATION)
        wlan.connect("uiot", "czcjhp1985cbm")

        -- 参数已配置完成，后台将自动开始连接wifi
        result, _ = sys.waitUntil("WLAN_READY")
        log.info("wlan", "WLAN_READY", result)
        -- 等待连上路由,此时还没获取到ip
        result, _ = sys.waitUntil("WLAN_STA_CONNECTED")
        log.info("wlan", "WLAN_STA_CONNECTED", result)
        -- 等到成功获取ip就代表连上局域网了
        result, data = sys.waitUntil("IP_READY")
        log.info("wlan", "IP_READY", result, data)

        while true do
            local httpc = esphttp.init(esphttp.GET, "http://httpbin.org/get")
            if httpc then
                local ok, err = esphttp.perform(httpc, true)
                log.info("httpc", "perform", ok, err)
                if ok then
                    local timeout = 120
                    ok = false
                    while timeout > 0 do
                        timeout = timeout - 20
                        local result, c, ret = sys.waitUntil("ESPHTTP_EVT", 20000)
                        log.info("httpc", "ESPHTTP_EVT", result, c, ret)
                        if c == httpc and ret == 5 then -- HTTP_EVENT_ON_FINISH
                            ok = true
                            break
                        end
                    end
                    if ok then
                        local code = esphttp.status_code(httpc)
                        log.info("esphttp", "code", code, "len", esphttp.content_length(httpc))
                        if code == 200 then
                            sys.wait(100)
                            log.info("esphttp", "data", esphttp.read_response(httpc, 1024))
                        end
                    else
                        log.info("esphttp", "http wait timeout")
                    end
                end
            else
                log.warn("http", "can't create httpc")
            end
            sys.wait(5000)
        end
    end
)
-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
