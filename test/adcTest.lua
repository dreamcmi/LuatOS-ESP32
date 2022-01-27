local adcTest = {}

local tag = "adcTest"

local adcChannels = {}

if MOD_TYPE == "air101" then
    adcChannels = {0, 1, 10}
elseif MOD_TYPE == "air103" then
    adcChannels = {0, 1, 2, 3}
elseif MOD_TYPE == "air105" then
    adcChannels = {1, 2, 4, 5, 6}
elseif MOD_TYPE == "ESP32C3" then
    adcChannels = {0, 1, 2, 3, 4}
end

function adcTest.test()
    if adc == nil then
        log.error(tag, "this fireware is not support adc")
        return
    end
    log.info(tag, "START")
    for _, id in pairs(adcChannels) do
        assert(adc.open(id) == true, tag .. ".open ERROR")
        log.info(tag .. ".read-" .. id, adc.read(id))
        adc.close(id)
    end
    log.info(tag, "DONE")

end

return adcTest
