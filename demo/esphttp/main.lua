PROJECT = "wifidemo"
VERSION = "1.0.0"

-- 引入必要的库文件(lua编写), 内部库不需要require
local sys = require "sys"

log.info("main", "ask for help", "https://wiki.luatos.com")

sys.taskInit(function ()
    while 1 do
        sys.wait(1000)
        --log.info("main", "fuck")
    end
end)

sys.taskInit(
    function()
        log.info("wlan", "wlan_init:", wlan.init())

        wlan.setMode(wlan.STATION)
        wlan.connect("switch_wlan", "1234567890", true) -- 第三参数是自动重连

        -- 参数已配置完成，后台将自动开始连接wifi

        -- 等到成功获取ip就代表连上局域网了
        result, data = sys.waitUntil("IP_READY")
        log.info("wlan", "IP_READY", result, data)

        while true do
            local httpc = esphttp.init(esphttp.GET, "http://httpbin.org/get")
            -- local httpc = esphttp.init(esphttp.GET, "https://www.howsmyssl.com") -- ssl测试,需要编译固件时打开#define ESPHTTP_USE_SSL 1
            if httpc then

                -- 异步写法
                local ok, err = esphttp.perform(httpc, true)
                if ok then
                    local count = 0
                    while 1 do
                        local result, c, ret, data = sys.waitUntil("ESPHTTP_EVT", 200)
                        --log.info("httpc", result, c, ret)
                        if c == httpc then
                            if esphttp.is_done(httpc, ret) then
                                break
                            end
                            if ret == esphttp.EVENT_ON_DATA and esphttp.status_code(httpc) == 200 then
                                log.info("data", "resp", #data)
                                count = count + #data
                            end
                        end
                    end
                    log.warn("esphttp", "resp count", count)
                else
                    log.warn("esphttp", "bad perform", err)
                end
                esphttp.cleanup(httpc)
            end
            sys.wait(3000)
        end
    end
)
-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
