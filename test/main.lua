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
    while true do
        require("adcTest").test()
        require("cameraTest").test()
        require("cryptoTest").test()
        require("dispTest").test()
        require("einkTest").test()
        require("fdbTest").test()
        require("fsTest").test()
        require("gpioTest").test()
        require("i2cTest").test()
        require("jsonTest").test()
        require("keyboardTest").test()
        require("lcdTest").test()
        require("lvglTest").test()
        require("mcuTest").test()
        require("packTest").test()
        require("pinTest").test()
        require("pwmTest").test()
        require("rtcTest").test()
        require("rtosTest").test()
        require("sdioTest").test()
        require("sfdTest").test()
        require("sfudTest").test()
        require("wlanTest").test()
        require("socketTest").test()
        require("spiTest").test()
        require("statemTest").test()
        require("stringTest").test()
        require("sysTest").test()
        require("touchkeyTest").test()
        require("u8g2Test").test()
        require("uartTest").test()
        require("wdtTest").test()
        require("zbuffTest").test()
        require("zlibTest").test()
        require("esphttpTest").test()
        require("mqttTest").test()
        require("bleTest").test()
        require("esp32Test").test()
        require("espnowTest").test()
        sys.wait(1000)
    end
end)

sys.run()
