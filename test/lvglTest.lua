local lvglTest = {}

local tag = "lvglTest"

function lvglTest.test()

    local spi_lcd = spi.deviceSetup(5, pin.PC14, 0, 0, 8, 96 * 1000 * 1000,
                                    spi.MSB, 1, 1)

    assert(spi_lcd ~= nil, tag .. ".deviceSetup ERROR")

    -- log.info("lcd.init",
    -- lcd.init("gc9a01",{port = "device",pin_dc = pin.PC12,pin_rst = pin.PC05,pin_pwr = pin.PC04,direction = 0,w = 240,h = 320,xoffset = 0,yoffset = 0},spi_lcd))
    -- log.info("lcd.init",
    -- lcd.init("st7789",{port = "device",pin_dc = pin.PC12, pin_rst = pin.PC05,pin_pwr = pin.PC04,direction = 0,w = 240,h = 240,xoffset = 0,yoffset = 0},spi_lcd))
    -- log.info("lcd.init",
    -- lcd.init("st7789",{port = "device",pin_dc = pin.PC12, pin_rst = pin.PC05,pin_pwr = pin.PC04,direction = 3,w = 240,h = 240,xoffset = 80,yoffset = 0},spi_lcd))
    -- log.info("lcd.init",
    -- lcd.init("st7789",{port = "device",pin_dc = pin.PC12, pin_rst = pin.PC05,pin_pwr = pin.PC04,direction = 3,w = 320,h = 240,xoffset = 0,yoffset = 0},spi_lcd))
    -- log.info("lcd.init",
    -- lcd.init("st7789",{port = "device",pin_dc = pin.PC12, pin_rst = pin.PC05,pin_pwr = pin.PC04,direction = 0,w = 240,h = 320,xoffset = 0,yoffset = 0},spi_lcd))
    -- log.info("lcd.init",
    -- lcd.init("st7735",{port = "device",pin_dc = pin.PC12, pin_rst = pin.PC05,pin_pwr = pin.PC04,direction = 0,w = 128,h = 160,xoffset = 2,yoffset = 1},spi_lcd))
    -- log.info("lcd.init",
    -- lcd.init("st7735v",{port = "device",pin_dc = pin.PC12,pin_rst = pin.PC05,pin_pwr = pin.PC04,direction = 1,w = 160,h = 80,xoffset = 0,yoffset = 24},spi_lcd))
    -- log.info("lcd.init",
    -- lcd.init("st7735s",{port = "device",pin_dc = pin.PC12,pin_rst = pin.PC05,pin_pwr = pin.PC04,direction = 2,w = 160,h = 80,xoffset = 1,yoffset = 26},spi_lcd))

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

    -- log.info("lcd.init",
    -- lcd.init("ili9341",{port = "device",pin_dc = pin.PC12, pin_rst = pin.PC05,pin_pwr = pin.PC04,direction = 0,w = 240,h = 320,xoffset = 0,yoffset = 0},spi_lcd))

    assert(lvgl.init() == true, tag .. ".lvgl.init ERROR")
    lvgl.disp_set_bg_color(nil, 0xFFFFFF)
    local scr = lvgl.obj_create(nil, nil)
    local btn = lvgl.btn_create(scr)
    lvgl.obj_align(btn, lvgl.scr_act(), lvgl.ALIGN_CENTER, 0, 0)
    local label = lvgl.label_create(btn)
    lvgl.label_set_text(label, "LuatOS!")
    lvgl.scr_load(scr)
end

return lvglTest
