local uartTest = {}

local tag = "uartTest"

local uartList = {}

if MOD_TYPE == "air101" then
    uartList = {}
elseif MOD_TYPE == "air103" then
    uartList = {}
elseif MOD_TYPE == "ESP32C3" then
    uartList = {1}
end

local receiveBuff = {}
local testData = string.rep(tag, 1)

local function getSerialData(id)
    local tmp = receiveBuff[id]
    receiveBuff[id] = ""
    return tmp
end

function uartTest.test()
    if uart == nil then
        log.error(tag, "this fireware is not support uart")
        return
    end
    log.info(tag, "START")
    for _, v in pairs(uartList) do
        receiveBuff[v] = ""
        assert(uart.setup(v, 115200, 8, 1) == 0, tag .. ".setup ERROR")
        uart.on(v, "receive", function(id, len)
            receiveBuff[id] = receiveBuff[id] .. uart.read(id, len)
            sys.publish("UART_RECEIVE_" .. v)
        end)
        assert(uart.write(v, testData) == #testData, tag .. ".write ERROR")
        sys.waitUntil("UART_RECEIVE_" .. v)
        assert(receiveBuff[v] == testData)
        uart.close(v)
    end
    log.info(tag, "DONE")
end

return uartTest
