PROJECT = "sdmmc"
VERSION = "1.0.0"

local sys = require "sys"

sys.taskInit(function()
    sys.wait(1000)
    --挂载sd卡
    sdmmc.init(2,8,-1,-1)
    sys.wait(1000)
    --写入sd卡
    if io.writeFile("/sdcard0/test.txt","bb") then
        log.debug("sdmmc","write suc")
    else
        log.debug("sdmmc","write fail")
    end
    sys.wait(1000)
    --读取写入的文件
    log.debug("sdmmc","read: "..io.readFile("/sdcard0/test.txt"))
    --卸载sd卡
    sdmmc.deinit()
    while 1 do
        sys.wait(1000)
    end
end)


-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
