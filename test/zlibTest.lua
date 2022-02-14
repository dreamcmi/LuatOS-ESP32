local zlibTest = {}

local tag = "zlibTest"

function zlibTest.test()
    if zlib == nil then
        log.error(tag, "this fireware is not support zlib")
        return
    end
    log.info(tag, "START")
    
    log.info(tag, "DONE")
end

return zlibTest
