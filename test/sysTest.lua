local sysTest = {}

local tag = "sysTest"

function sysTest.test()
    if sys == nil then
        log.error(tag, "this fireware is not support sys")
        return
    end
    log.info(tag, "START")
    
    log.info(tag, "DONE")
end

return sysTest
