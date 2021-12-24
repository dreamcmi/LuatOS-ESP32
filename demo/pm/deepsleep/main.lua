PROJECT = "pm"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

-- gpio唤醒
sys.taskInit(
    function()
        while 1 do
            log.info("deepsleep", "I want to sleep, press gpio5 to wake me up")
            esp32.enterDeepSleep(esp32.GPIO, 5, 0) --只能是rtc gpio
            log.info("wakeup", "gpio5")
            sys.wait(1000) -- 硬核消抖？
        end
    end
)

-- rtc唤醒
-- sys.taskInit(
--     function()
--         while 1 do
--             log.info("deepsleep", "I want sleep 10s")
--             esp32.enterDeepSleep(esp32.RTC, 10 * 1000 * 1000)
--             log.info("wakeup", "10s")
--         end
--     end
-- )

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
