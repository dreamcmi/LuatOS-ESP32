local i2cTest = {}

local tag = "i2cTest"
local i2cId
if MOD_TYPE == "air101" or MOD_TYPE == "air103" or MOD_TYPE == "air105" or
    MOD_TYPE == "ESP32C3" then i2cId = 0 end

function i2cTest.test()
    if i2c == nil then
        log.error(tag, "this fireware is not support i2c")
        return
    end
    -- AHT10
    local i2cSlaveAddr = 0x38
    log.info(tag, "START")
    assert(i2c.setup(i2cId) == 0, tag .. ".setup ERROR")
    assert(i2c.send(i2cId, i2cSlaveAddr, string.char(0xac, 0x22, 0x00)) == true,
           tag .. ".send ERROR")
    local receivedData = i2c.recv(i2cId, i2cSlaveAddr, 6)
    assert(#receivedData == 6, tag .. ".recv ERROR")

    -- log.info(tag .. ".receivedDataHex", receivedData:toHex())
    local tempBit = string.byte(receivedData, 6) + 0x100 *
                        string.byte(receivedData, 5) + 0x10000 *
                        (string.byte(receivedData, 4) & 0x0F)
    local humidityBit = (string.byte(receivedData, 4) & 0xF0) + 0x100 *
                            string.byte(receivedData, 3) + 0x10000 *
                            string.byte(receivedData, 2)
    -- log.info(tag .. ".tempBit", tempBit)
    -- log.info(tag .. ".humidityBit", humidityBit)
    humidityBit = humidityBit >> 4
    local calcTemp = (tempBit / 1048576) * 200 - 50
    local calcHum = humidityBit / 1048576
    log.info(tag .. ".当前温度", string.format("%.1f℃", calcTemp))
    log.info(tag .. ".当前湿度", string.format("%.1f%%", calcHum * 100))

    i2c.close(i2cId)

    log.info(tag, "DONE")

    -- SHT30
    -- local addr = 0x44

    -- assert(i2c.setup(0) == 0, tag .. ".setup ERROR")

    -- assert(i2c.send(i2cId, addr, string.char(0x2C, 0x06)) == true,
    --        tag .. ".send ERROR")
    -- local data = i2c.recv(i2cId, addr, 6)
    -- assert(#data == 6, tag .. ".recv ERROR")
    -- local _, tval, ccrc, hval, hcrc = pack.unpack(data, ">HbHb")

    -- local calcTemp = ((tval * 175) / 65535.0) - 45
    -- local calcHum = ((hval * 100) / 65535.0)
    -- log.info(tag .. ".temp,hum", calcTemp, calcHum)
    -- i2c.close(i2cId)

end

return i2cTest
