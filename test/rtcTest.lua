local rtcTest = {}

local tag = "rtcTest"

function rtcTest.test()
    if rtc == nil then
        log.error(tag, "this fireware is not support rtc")
        return
    end
    log.info(tag, "START")
    assert(
        rtc.set({year = 2021, mon = 2, day = 20, hour = 0, min = 0, sec = 0}) ==
            true, tag .. ".set ERROR")
    log.info(tag .. ".get", json.encode(rtc.get()))
    log.info(tag, "DONE")
end

return rtcTest
