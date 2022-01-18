local pinTest = {}

local tag = "pinTest"

local pinList = {}

if MOD_TYPE == "air101" then
    pinList = {
        PA00 = 0,
        PA01 = 1,
        PA04 = 4,
        PA07 = 7,
        PB00 = 16,
        PB01 = 17,
        PB02 = 18,
        PB03 = 19,
        PB04 = 20,
        PB05 = 21,
        PB06 = 22,
        PB07 = 23,
        PB08 = 24,
        PB09 = 25,
        PB10 = 26,
        PB11 = 27
    }
elseif MOD_TYPE == "air103" then
    pinList = {
        PA01 = 01,
        PA02 = 02,
        PA03 = 03,
        PA04 = 04,
        PA05 = 05,
        PA06 = 06,
        PA07 = 07,
        PA08 = 08,
        PA09 = 09,
        PA10 = 10,
        PA11 = 11,
        PA12 = 12,
        PA13 = 13,
        PA14 = 14,
        PA15 = 15,
        PB00 = 16,
        PB01 = 17,
        PB02 = 18,
        PB03 = 19,
        PB04 = 20,
        PB05 = 21,
        PB06 = 22,
        PB07 = 23,
        PB08 = 24,
        PB09 = 25,
        PB10 = 26,
        PB11 = 27,
        PB12 = 28,
        PB13 = 29,
        PB14 = 30,
        PB15 = 31,
        PB16 = 32,
        PB17 = 33,
        PB18 = 34,
        PB21 = 37,
        PB22 = 38,
        PB24 = 40,
        PB25 = 41,
        PB26 = 42
    }
end

function pinTest.test()
    if pin == nil then
        log.error(tag, "this fireware is not support pin")
        return
    end
    log.info(tag, "START")
    for k, v in pairs(pinList) do
        assert(pin.get(k) == v, tag .. ".get ERROR")
    end
    log.info(tag, "DONE")
end

return pinTest
