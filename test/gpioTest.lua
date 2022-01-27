local gpioTest = {}

local tag = "gpioTest"
local gpioList = {}
if MOD_TYPE == "air101" then
    gpioList = {0, 1, 4, 7, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27}
elseif MOD_TYPE == "air103" then
    gpioList = {
        01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 37, 38,
        40, 41, 42
    }
elseif MOD_TYPE == "air105" then
    gpioList = {
        2, 3, 5, 6, 7, 8, 9, 10, 16, 17, 18, 19, 20, 21, 28, 29, 30, 31, 32, 33,
        36, 37, 38, 39, 40, 41, 44, 45, 36, 37, 49, 50, 51, 52, 53, 56, 57, 58,
        59, 60, 61, 62, 63, 64, 65, 66, 70, 71, 72, 73, 74, 75
    }
elseif MOD_TYPE == "ESP32C3" then
    gpioList = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 18, 19}
end

function gpioTest.test()
    if gpio == nil then
        log.error(tag, "this fireware is not support gpio")
        return
    end
    log.info(tag, "START")
    for _, v in pairs(gpioList) do
        assert(gpio.setDefaultPull(1) == true, tag .. ".setDefaultPull ERROR")
        gpio.setup(v, 1)
        assert(gpio.get(v) == 1, tag .. ".get ERROR " .. v)
        gpio.set(v, 0)
        assert(gpio.get(v) == 0, tag .. ".get ERROR " .. v)
        assert(gpio.setDefaultPull(2) == true, tag .. ".setDefaultPull ERROR")
        gpio.setup(v, 0)
        assert(gpio.get(v) == 0, tag .. ".get ERROR " .. v)
        assert(gpio.setDefaultPull(1) == true, tag .. ".setDefaultPull ERROR")
        gpio.setup(v, nil)
        assert(gpio.get(v) == 1, tag .. ".get ERROR " .. v)
        assert(gpio.setDefaultPull(2) == true, tag .. ".setDefaultPull ERROR")
        gpio.setup(v, nil)
        assert(gpio.get(v) == 0, tag .. ".get ERROR " .. v)
        assert(gpio.setDefaultPull(1) == true, tag .. ".setDefaultPull ERROR")
        gpio.setup(v, function()
            log.info(tag .. ".int-" .. v, gpio.get(v))
        end, gpio.PULLUP)
        gpio.close(v)
    end
    log.info(tag, "DONE")
end

return gpioTest
