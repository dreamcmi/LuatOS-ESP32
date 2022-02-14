local touchkeyTest = {}

local tag = "touchkeyTest"

function touchkeyTest.test()
    if touchkey == nil then
        log.error(tag, "this fireware is not support touchkey")
        return
    end
    log.info(tag, "START")
    
    log.info(tag, "DONE")
end

return touchkeyTest
