PROJECT = "adcdemo"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

sys.taskInit(
    function()
        for i = 0, 4 do
            adc.open(i)
        end
        while 1 do
            for i = 0, 4 do
                log.info("adc" .. i, adc.read(i))
            end
            sys.wait(1000)
        end
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
