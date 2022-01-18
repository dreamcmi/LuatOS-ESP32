PROJECT = "LuatOS-SoC-Test"
VERSION = "1.0.0"
MOD_TYPE = rtos.bsp()
log.info("MOD_TYPE", MOD_TYPE)

function printTable(tbl, lv)
    lv = lv and lv .. "\t" or ""
    print(lv .. "{")
    for k, v in pairs(tbl) do
        if type(k) == "string" then k = "\"" .. k .. "\"" end
        if "string" == type(v) then
            local qv = string.match(string.format("%q", v), ".(.*).")
            v = qv == v and '"' .. v .. '"' or "'" .. v:toHex() .. "'"
        end
        if type(v) == "table" then
            print(lv .. "\t" .. tostring(k) .. " = ")
            printTable(v, lv)
        else

            print(lv .. "\t" .. tostring(k) .. " = " .. tostring(v) .. ",")
        end
    end
    print(lv .. "},")
end

sys = require("sys")

sys.taskInit(function()
    require("adcTest").test()
    require("cryptoTest").test()
    require("fdbTest").test()
    require("i2cTest").test()
    require("fsTest").test()
    require("gpioTest").test()
    require("jsonTest").test()
    require("pwmTest").test()
    require("mcuTest").test()
    require("pinTest").test()
    require("rtcTest").test()
    require("rtosTest").test()
    require("stringTest").test()
    require("zbuffTest").test()
    require("wlanTest").test()
    require("socketTest").test()
    require("mqttTest").test()
    require("bleTest").test()
    require("esp32Test").test()
    require("espnowTest").test()
    require("cameraTest").test()
    require("uartTest").test()
end)

sys.run()
