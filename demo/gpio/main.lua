PROJECT = "gpiodemo"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

sys.taskInit(
    function()
        local G8 = gpio.setup(8, 0) -- 输出模式
        while 1 do
            G8(0)
            sys.wait(1000)
            G8(1)
            sys.wait(1000)
        end
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
