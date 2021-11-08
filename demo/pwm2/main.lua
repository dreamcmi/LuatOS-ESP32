PROJECT = "pwm2"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

sys.taskInit(
    function()
        ch = pwm2.init(0, 0, 10, 10, 1 * 1000, 1)
        log.info("pwm", "channel", ch)
        pwm2.setFadeWithTime(ch, 500, 10, 1)
        sys.wait(1000)
        pwm2.setFadeWithTime(ch, 300, 10, 1)
        sys.wait(1000)
        pwm2.setFadeWithTime(ch, 800, 10, 1)
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!