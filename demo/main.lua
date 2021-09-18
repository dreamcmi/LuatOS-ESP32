
-- LuaTools需要PROJECT和VERSION这两个信息
PROJECT = "demo"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"


sys.taskInit(function()
    if i2c.setup(0, i2c.FAST, 0x38) == 1 then
        log.info("存在 i2c0")
    else
        i2c.close(0) -- 关掉
        log.info("i2c0打开失败")
    end
    while true do
        log.info("esp32c3", "Go Go Go ~")
        sys.wait(1000)
    end
end)

sys.taskInit(function()
    local G8 = gpio.setup(8, 0) -- 输出模式
    while 1 do
        G8(0)
        sys.wait(1000)
        G8(1)
        sys.wait(1000)
    end
end)

sys.taskInit(function ()
    for i=0,4 do
        adc.open(i)
    end

    while 1 do
        for i=0,4 do
            log.info("adc"..i,adc.read(i))
        end
        sys.wait(1000)
    end
end)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
