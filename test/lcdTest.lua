local lcdTest = {}

local tag = "lcdTest"

function lcdTest.test()
    if lcd == nil then
        log.error(tag, "this fireware is not support lcd")
        return
    end
    log.info(tag, "START")
    local spiLcd
    if MOD_TYPE == "air101" then
        spiLcd = spi.deviceSetup(5, pin.PC14, 0, 0, 8, 96 * 1000 * 1000,
                                 spi.MSB, 1, 1)
    elseif MOD_TYPE == "air105" then
        spiLcd = spi.deviceSetup(5, pin.PC14, 0, 0, 8, 96 * 1000 * 1000,
                                 spi.MSB, 1, 1)
    elseif MOD_TYPE == "ESP32C3" then
        spiLcd = spi.deviceSetup(2, 7, 0, 0, 8, 60 * 1000 * 1000, spi.MSB, 1, 1)
    end
    assert(spiLcd ~= nil, tag .. ".deviceSetup ERROR")
    if MOD_TYPE == "air101" then
    elseif MOD_TYPE == "air105" then
        assert(lcd.init("gc9306", {
            port = "device",
            pin_dc = pin.PE8,
            pin_rst = pin.PC12,
            pin_pwr = pin.PE9,
            direction = 0,
            w = 240,
            h = 320,
            xoffset = 0,
            yoffset = 0
        }, spiLcd) == true, tag .. ".lcd.init ERROR")
    elseif MOD_TYPE == "ESP32C3" then
        assert(lcd.init("gc9306", {
            port = "device",
            pin_dc = 6,
            pin_rst = 10,
            pin_pwr = 11,
            direction = 0,
            w = 240,
            h = 320,
            xoffset = 0,
            yoffset = 0
        }, spiLcd) == true, tag .. ".lcd.init ERROR")
        -- assert(lcd.init("gc9106l", {
        --     port = "device",
        --     pin_dc = 6,
        --     pin_rst = 10,
        --     pin_pwr = 11,
        --     direction = 0,
        --     w = 128,
        --     h = 160,
        --     xoffset = 100,
        --     yoffset = 100
        -- }, spiLcd) == true, tag .. ".lcd.init ERROR")
        -- assert(lcd.init("st7735", {
        --     port = "device",
        --     pin_dc = 6,
        --     pin_rst = 10,
        --     pin_pwr = 11,
        --     direction = 0,
        --     w = 128,
        --     h = 160,
        --     xoffset = 2,
        --     yoffset = 1
        -- }, spiLcd) == true, tag .. ".lcd.init ERROR")
    end
    log.info("lcd.drawLine", lcd.drawLine(20, 20, 150, 20, 0x001F))
    log.info("lcd.drawRectangle", lcd.drawRectangle(20, 40, 120, 70, 0xF800))
    log.info("lcd.drawCircle", lcd.drawCircle(50, 50, 20, 0x0CE0))
    log.info(tag .. ".off", "关闭背光")
    lcd.off()
    sys.wait(2000)
    log.info(tag .. ".on", "开启背光")
    lcd.on()
    sys.wait(2000)
    log.info(tag .. ".invon", "开启反显")
    lcd.invon()
    sys.wait(1000)
    log.info(tag .. ".invoff", "关闭反显")
    lcd.invoff()
    lcd.sleep()
    lcd.wakeup()
    lcd.setColor(0x0000, 0xFFFF)
    sys.wait(2000)
    lcd.setColor(0xFFFF, 0x0000)
    sys.wait(2000)
    log.info(tag .. ".clear", "黑色清屏")
    lcd.clear(0x0000)
    sys.wait(2000)
    log.info(tag .. ".clear", "红色清屏")
    lcd.clear(0xf800)
    sys.wait(2000)
    log.info(tag .. ".clear", "绿色清屏")
    lcd.clear(0x0400)
    sys.wait(2000)
    log.info(tag .. ".clear", "蓝色清屏")
    lcd.clear(0x001f)
    sys.wait(2000)
    log.info(tag .. ".clear", "白色清屏")
    lcd.clear(0xFFFF)
    sys.wait(2000)
    -- lcd.draw(20, 30, 220, 33, zbuff.create({201, 4, 16}, 0x001F))

    -- for i = 239, 1, -1 do
    --     print(i)
    --     lcd.clear()
    --     local zbuff = zbuff.create({i + 1, 20, 16}, 0x001F)
    --     lcd.draw(0, 0, i, 19, zbuff)
    --     zbuff = nil
    --     collectgarbage("collect")
    --     sys.wait(100)
    -- end
    -- lcd.draw(20, 30, 120, 33, zbuff.create({101, 4, 16}, 0x001F))

    log.info(tag .. ".close", "关闭显示屏")
    lcd.close()
    log.info(tag, "DONE")
end

return lcdTest
