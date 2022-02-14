local sdioTest = {}

local tag = "sdioTest"

function sdioTest.test()
    if sdio == nil then
        log.error(tag, "this fireware is not support sdio")
        return
    end
    log.info(tag, "START")
    
    log.info(tag, "DONE")
end

return sdioTest
