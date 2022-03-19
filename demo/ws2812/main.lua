PROJECT = "ws2812"
VERSION = "1.0.0"

local sys = require "sys"
local ws2812Pin = 8

sys.taskInit(
    function()
        log.info("rmt.init", rmt.init(ws2812Pin, 0, rmt.TX, 2)) -- 建议二分频
        handle = rmt.ws2812Init(10, 0)
        assert(handle ~= nil, "ws2812.init error")

        log.info("ws2812.clear", rmt.ws2812Clear(handle))
        for i = 0, 9 do
            log.info("ws2812.setpixel" .. i, rmt.ws2812SetPixel(handle, i, 255, 0, 0))
        end
        log.info("ws2812.refresh", rmt.ws2812Refresh(handle))

        sys.wait(3000)

        log.info("ws2812.clear", rmt.ws2812Clear(handle))
        for i = 0, 9 do
            log.info("ws2812.setpixel" .. i, rmt.ws2812SetPixel(handle, i, 0, 255, 0))
        end
        log.info("ws2812.refresh", rmt.ws2812Refresh(handle))

        sys.wait(3000)

        log.info("ws2812.clear", rmt.ws2812Clear(handle))
        for i = 0, 9 do
            log.info("ws2812.setpixel" .. i, rmt.ws2812SetPixel(handle, i, 0, 0, 255))
        end
        log.info("ws2812.refresh", rmt.ws2812Refresh(handle))
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
