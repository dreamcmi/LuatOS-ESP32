local dispTest = {}

local tag = "dispTest"

-- 对接SSD1306
function display_str(str)
    disp.clear()
    disp.drawStr(str, 1, 18)
    disp.update()
end

function ui_update()
    disp.clear() -- 清屏

    disp.drawStr(os.date("%Y-%m-%d %H:%M:%S"), 1, 12) -- 写日期

    disp.drawStr("Luat@Air101" .. " " .. _VERSION, 1, 24) -- 写版本号

    disp.update()
end

function dispTest.test()

    log.info("disp", "init ssd1306")
    disp.init({mode = "i2c_sw", pin0 = 1, pin1 = 4})
    disp.setFont(1)
    display_str("启动中 ...")

    sys.taskInit(function()
        while 1 do
            sys.wait(1000)
            ui_update()
        end
    end)
end

return dispTest
