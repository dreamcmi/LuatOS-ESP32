local mcuTest = {}

local tag = "mcuTest"

local clkList = {}

if MOD_TYPE == "air101" or MOD_TYPE == "air103" then clkList = {2, 80, 240} end

function mcuTest.test()
    if mcu == nil then
        log.error(tag, "this fireware is not support mcu")
        return
    end
    log.info(tag, "START")
    if MOD_TYPE == "air101" or MOD_TYPE == "air103" then
        for _, v in pairs(clkList) do
            assert(mcu.setClk(v) == true, tag .. ".setClk ERROR")
            assert(mcu.getClk(v) == v, tag .. ".getClk ERROR")
        end
    end
    log.info(tag .. ".unique_id", string.toHex(mcu.unique_id()))
    log.info(tag .. ".ticks", mcu.ticks())
    log.info(tag, "DONE")
end

return mcuTest
