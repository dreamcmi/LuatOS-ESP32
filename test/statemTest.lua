local statemTest = {}

local tag = "statemTest"

function statemTest.test()
    if statem == nil then
        log.error(tag, "this fireware is not support statem")
        return
    end
    log.info(tag, "START")
    
    log.info(tag, "DONE")
end

return statemTest
