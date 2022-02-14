local einkTest = {}

local tag = "einkTest"

function einkTest.test()
    if eink == nil then
        log.error(tag, "this fireware is not support eink")
        return
    end
    log.info(tag, "START")
    eink.model(eink.MODEL_1in54)
    if MOD_TYPE == "air101" then
        assert(eink.setup(0, 0, 16, 19, 17, 20) == true, tag .. ".setup ERROR")
    elseif MOD_TYPE == "ESP32C3" then
        assert(eink.setup(0, 2, 11, 10, 6, 7) == true, tag .. ".setup ERROR")
    end
    local width, height, rotate = 200, 200, 1
    eink.setWin(width, height, rotate)
    local getWidth, getHeight, getRotate = eink.getWin()
    assert(getWidth == width and getHeight == height and getRotate == rotate,
           tag .. "getWin ERROR")
    eink.clear()
    eink.print(0, 12, tag, 0, eink.font_opposansm12)
    eink.print(0, 40, "墨水屏中文显示测试", 0,
               eink.font_opposansm16_chinese)
    eink.line(0, 0, 200, 200, 0)
    eink.rect(50, 50, 100, 100, 0, 1)
    eink.circle(120, 120, 20, 0, 1)
    eink.qrcode(0, 50, tag, 1)
    eink.bat(0, 80, 5000)
    eink.weather_icon(0, 100, 401)
    eink.weather_icon(50, 100, 302)
    eink.weather_icon(0, 150, 503)
    eink.weather_icon(50, 150, 501)
    eink.weather_icon(100, 50, 504)
    eink.weather_icon(150, 50, 103)
    eink.weather_icon(150, 100, 306)
    eink.weather_icon(100, 150, 101)
    eink.weather_icon(150, 150, 100)
    eink.show()
    log.info(tag, "DONE")
end

return einkTest
