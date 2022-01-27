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
elseif MOD_TYPE == "air105" then
    pinList = {
        PA2 = 2,
        PA3 = 3,
        PA5 = 5,
        PA6 = 6,
        PA7 = 7,
        PA8 = 8,
        PA9 = 9,
        PA10 = 10,
        PB0 = 16,
        PB1 = 17,
        PB2 = 18,
        PB3 = 19,
        PB4 = 20,
        PB5 = 21,
        PB12 = 28,
        PB13 = 29,
        PB14 = 30,
        PB15 = 31,
        PC0 = 32,
        PC1 = 33,
        PC4 = 36,
        PC5 = 37,
        PC6 = 38,
        PC7 = 39,
        PC8 = 40,
        PC9 = 41,
        PC12 = 44,
        PC13 = 45,
        PC14 = 36,
        PC15 = 37,
        PD1 = 49,
        PD2 = 50,
        PD3 = 51,
        PD6 = 52,
        PD7 = 53,
        PD8 = 56,
        PD9 = 57,
        PD10 = 58,
        PD11 = 59,
        PD12 = 60,
        PD13 = 61,
        PD14 = 62,
        PD15 = 63,
        PE0 = 64,
        PE1 = 65,
        PE2 = 66,
        PE6 = 70,
        PE7 = 71,
        PE8 = 72,
        PE9 = 73,
        PE10 = 74,
        PE11 = 75
    }
end

function pinTest.test()
    if pin == nil then
        log.error(tag, "this fireware is not support pin")
        return
    end
    log.info(tag, "START")
    for k, v in pairs(pinList) do
        print(k, pin.get(k))
        assert(pin.get(k) == v, tag .. ".get ERROR")
    end
    log.info(tag, "DONE")
end

return pinTest
