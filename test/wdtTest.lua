local wdtTest = {}

local tag = "wdtTest"

function wdtTest.test()
    if wdt == nil then
        log.error(tag, "this fireware is not support wdt")
        return
    end
    log.info(tag, "START")
    
    log.info(tag, "DONE")
end

return wdtTest
