local socketTest = {}

local tag = "socketTest"

local NEEDWIFICONNECT = false
local testData = string.rep("socketTest", 100)
local ip, port = "47.96.229.157", 2901
-- local ip, port = "112.125.89.8", 33267
local waitRes, data
local recvData, recvLen = "", 0

local function SocketTestTask(protocol, ip, port, data)
    local socketClient = socket.create(protocol)
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
    if NEEDWIFICONNECT == true then
        assert(wlan.init() == 0, tag .. ".init ERROR")
        assert(wlan.setMode(wlan.STATION) == 0, tag .. ".setMode ERROR")
        assert(wlan.connect("Xiaomi_AX6000", "Air123456") == 0,
               tag .. ".connect ERROR")
        waitRes, data = sys.waitUntil("WLAN_STA_CONNECTED", 10000)
        log.info(tag, "WLAN_STA_CONNECTED", waitRes, data)
        waitRes, data = sys.waitUntil("IP_READY", 10000)
        log.info(tag, "IP_READY", waitRes, data)
    end
    SocketTestTask(socket.TCP, ip, port, testData)
    SocketTestTask(socket.UDP, ip, port, testData)
    if NEEDWIFICONNECT == true then
        assert(wlan.disconnect() == 0, tag .. ".disconnect ERROR")
        waitRes, data = sys.waitUntil("WLAN_STA_DISCONNECTED", 10000)
        log.info(tag .. ".WLAN_STA_DISCONNECTED", waitRes, data)
    end
    log.info(tag, "DONE")
end

return socketTest
