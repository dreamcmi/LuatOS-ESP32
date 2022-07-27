PROJECT = "pwm2"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

-- pwm2初始化使用注意事项
-- 第一个参数为定时器 可选0-3 如果是多路相同频率可选同一个定时器
-- 第二个参数为通道 可选0-5 共计六个通道
-- 第三个参数为引脚 随意指定
-- 第四个参数为分辨率 原则:频率低高分辨率 频率高低分辨率 (如50hz 需要开13bit)
-- 第五个参数为频率 单位hz
-- 第六个参数为渐入渐出 用于led调光渐变 不需要可不填

-- 舵机 控制线接IO9
sys.taskInit(
    function()
        -- 舵机驱动原理 20ms一周期 对应50hz 0.5ms高电平表示0度 2.5ms高电平表示180度
        ch = pwm2.init(0, 0, 9, 13, 50) -- 定时器0 通道0 引脚IO9 分辨率13bit 频率50hz

        while 1 do
            pwm2.setDuty(ch, 204) -- 0.5ms 0度
            sys.wait(1000)
            pwm2.setDuty(ch, 204 * 2) -- 1ms 45度
            sys.wait(1000)
            pwm2.setDuty(ch, 204 * 3) -- 1.5ms 90度
            sys.wait(1000)
            pwm2.setDuty(ch, 204 * 4) -- 2ms 135度
            sys.wait(1000)
            pwm2.setDuty(ch, 204 * 5) -- 2.5ms 180度
            sys.wait(1000)
        end
    end
)

-- led
-- sys.taskInit(
--     function()
--         ch = pwm2.init(0, 0, 9, 10, 1 * 1000, 1)
--         log.info("pwm", "channel", ch)
--         pwm2.setFadeWithTime(ch, 500, 10, 1)
--         sys.wait(1000)
--         pwm2.setFadeWithTime(ch, 300, 10, 1)
--         sys.wait(1000)
--         pwm2.setFadeWithTime(ch, 800, 10, 1)
--     end
-- )

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
