local rtosTest = {}

local tag = "rtosTest"

function rtosTest.test()
    if rtos == nil then
        log.error(tag, "this fireware is not support rtos")
        return
    end
    log.info(tag, "START")
    log.info(tag .. ".buildDate", rtos.buildDate())
    log.info(tag .. ".bsp", rtos.bsp())
    log.info(tag .. ".version", rtos.version())
    log.info(tag .. ".meminfo", rtos.meminfo("sys"))
    log.info(tag .. ".meminfo", rtos.meminfo("lua"))
    log.info(tag .. ".firmware", rtos.firmware())
    log.info(tag, "DONE")
end

return rtosTest
