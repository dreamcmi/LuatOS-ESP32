local lcdTest = {}

local tag = "lcdTest"

function lcdTest.test()

    local spi_lcd = spi.deviceSetup(5, pin.PC14, 0, 0, 8, 96 * 1000 * 1000,
                                    spi.MSB, 1, 1)

    assert(spi_lcd ~= nil, tag .. ".deviceSetup ERROR")

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
    }, spi_lcd) == true, tag .. ".lcd.init ERROR")

    log.info("lcd.drawLine", lcd.drawLine(20, 20, 150, 20, 0x001F))
    log.info("lcd.drawRectangle", lcd.drawRectangle(20, 40, 120, 70, 0xF800))
    log.info("lcd.drawCircle", lcd.drawCircle(50, 50, 20, 0x0CE0))

end

return lcdTest
