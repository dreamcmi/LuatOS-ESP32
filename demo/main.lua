
-- LuaTools需要PROJECT和VERSION这两个信息
PROJECT = "demo"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"


sys.taskInit(function()
    while 1 do
        log.info("HELLO WORLD", "Go Go Go")
        sys.wait(1000)
    end
end)


-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
