local sfdTest = {}

local tag = "sfdTest"

function sfdTest.test()
    if sfd == nil then
        log.error(tag, "this fireware is not support sfd")
        return
    end
    log.info(tag, "START")
    
    log.info(tag, "DONE")
end

return sfdTest
