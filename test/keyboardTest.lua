local keyboardTest = {}

local tag = "keyboardTest"

function keyboardTest.test()
    keyboard.init(0, 0x1F, 0x14)
    sys.subscribe("KB_INC", function(port, data, state)
        -- port 当前固定为0, 可以无视
        -- data, 需要配合init的map进行解析
        -- state, 1 为按下, 0 为 释放
        log.info("keyboard", port, data, state)
    end)
end

return keyboardTest
