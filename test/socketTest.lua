local socketTest = {}

local tag = "socketTest"

local testData = string.rep("socketTest", 100)
-- local ip, port = "airtest.openluat.com", 2901
local ip, port = "112.125.89.8", 33267
local waitRes, data
local recvData, recvLen = "", 0

local function SocketTest(protocol, ip, port, data)
    local socketClient = socket.creat(protocol)
    while true do
        local errCode = socket.connect(socketClient, ip, port)
        if errCode == 0 then
            break
        else
            log.error(tag, ".connect FAIL", errCode)
            sys.wait(1000)
        end
    end
    assert(socket.send(socketClient, data) == #data, tag .. ".send ERROR")
    local tmp, len
    while true do
        tmp, len = socket.recv(socketClient)
        if tmp ~= nil then
            recvData = recvData .. tmp
            recvLen = recvLen + #tmp
        end
        if recvLen >= #data then break end
        sys.wait(100)
    end
    assert(recvData == data, tag .. ".recv ERROR")
    socket.close(socketClient)
end

function socketTest.test()
    if socket == nil then
        log.error(tag, "this fireware is not support socket")
        return
    end
    log.info(tag, "START")
    assert(wlan.init() == 0, tag .. ".init ERROR")
    assert(wlan.setMode(wlan.STATION) == 0, tag .. ".setMode ERROR")
    assert(wlan.connect("Xiaomi_AX6000", "Air123456") == 0,
           tag .. ".connect ERROR")
    waitRes, data = sys.waitUntil("WLAN_READY", 10000)
    log.info(tag, "WLAN_READY", waitRes, data)
    waitRes, data = sys.waitUntil("WLAN_STA_CONNECTED", 10000)
    log.info(tag, "WLAN_STA_CONNECTED", waitRes, data)
    waitRes, data = sys.waitUntil("IP_READY", 10000)
    log.info(tag, "IP_READY", waitRes, data)
    SocketTest(socket.TCP, ip, port, testData)
    SocketTest(socket.UDP, ip, 36516, testData)
    assert(wlan.disconnect() == 0, tag .. ".disconnect ERROR")
    waitRes, data = sys.waitUntil("WLAN_STA_DISCONNECTED", 10000)
    log.info(tag, "WLAN_STA_DISCONNECTED", waitRes, data)
    assert(wlan.deinit() == 0, tag .. ".deinit ERROR")
    log.info(tag, "DONE")
end

return socketTest
