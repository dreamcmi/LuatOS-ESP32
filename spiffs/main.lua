PROJECT = "esp32lib"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

sys.taskInit(
    function()
        log.info("esp32", "rstReason",esp32.getRstReason())
        local re = esp32.getchip()
        log.info("esp32", "chip", re["chip"])
        log.info("esp32", "features", re["features"])
        log.info("esp32", "cores", re["cores"])
        log.info("esp32", "revision", re["revision"])

        stamac = esp32.getmac(0)
        log.info("stamac", string.toHex(stamac))

        apmac = esp32.getmac(1)
        log.info("apmac", string.toHex(apmac))

        btmac = esp32.getmac(2)
        log.info("btmac", string.toHex(btmac))

        ethmac = esp32.getmac(3)
        log.info("ethmac", string.toHex(ethmac))

        while 1 do
            log.info("esp32","random",esp32.random())
            sys.wait(3000)
        end
    end
)


-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
