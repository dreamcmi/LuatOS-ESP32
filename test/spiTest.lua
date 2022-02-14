local spiTest = {}

local tag = "spiTest"

function spiTest.test()
    if spi == nil then
        log.error(tag, "this fireware is not support spi")
        return
    end
    log.info(tag, "START")
    
    log.info(tag, "DONE")
end

return spiTest
