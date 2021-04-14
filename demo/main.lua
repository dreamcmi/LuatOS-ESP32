
-- LuaTools需要PROJECT和VERSION这两个信息
PROJECT = "demo"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"


sys.taskInit(function()
    gpio.setup(18,1)
    log.info("18", "start")
    sys.wait(1000)
    while 1 do
        gpio.set(18,0)
        log.info("18", "0")
        sys.wait(1000)

        gpio.set(18,1)
        log.info("18", "1")
        sys.wait(1000)
    end
end)


-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
