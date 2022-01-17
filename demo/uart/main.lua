PROJECT = "uartdemo"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"
local uartid = 1

sys.taskInit(
    function()
        local result =
            uart.setup(
            uartid,
            --串口id
            115200,
            --波特率
            8,
            --数据位
            1
            --停止位
        )
        log.info("uart-setup", result)
        uart.write(uartid, "hello esp32\n")
        uart.on(
            uartid,
            "receive",
            function(id, len)
                local s = ""
                s = uart.read(id, len)
                log.info("uart", id, len)
                if #s > 0 then -- #s 是取字符串的长度
                    log.info("uart", "receive", id, #s, s)
                end
            end
        )
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
