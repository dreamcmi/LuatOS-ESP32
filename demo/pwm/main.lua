PROJECT = "pwm"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

sys.taskInit(function()
    while 1 do
        -- 仿呼吸灯效果
        log.info("pwm", ">>>>>")
        for i = 10,1,-1 do 
            pwm.open(0, 1000, i*9) -- 5 通道, 频率1000hz, 占空比0-100
            sys.wait(200 + i*10)
        end
        for i = 10,1,-1 do 
            pwm.open(0, 1000, 100 - i*9)
            sys.wait(200 + i*10)
        end
        sys.wait(5000)
    end
end)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
