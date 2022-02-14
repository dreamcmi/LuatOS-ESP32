local u8g2Test = {}

local tag = "u8g2Test"

function u8g2Test.test()
    if u8g2 == nil then
        log.error(tag, "this fireware is not support u8g2")
        return
    end
    log.info(tag, "START")
    
    log.info(tag, "DONE")
end

return u8g2Test
