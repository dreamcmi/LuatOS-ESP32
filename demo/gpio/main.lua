PROJECT = "gpiodemo"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

sys.taskInit(
    function()
        local LED_D4 = gpio.setup(12, 0)
        local LED_D5 = gpio.setup(13, 0)
        while 1 do
            LED_D4(0)
            LED_D5(1)
            sys.wait(1000)
            LED_D4(1)
            LED_D5(0)
            sys.wait(1000)
        end
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
