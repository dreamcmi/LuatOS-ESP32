PROJECT = "ble"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

sys.taskInit(function ( ... )
    ret = ble.init()
    log.info("ble", "ble_init:",ret)
    sys.wait(10*1000)
    ret2 = ble.deinit()
    log.info("ble", "ble_deinit",ret2)
end)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
