local pwmTest = {}

local tag = "pwmTest"
if MOD_TYPE == "air101" then
    pwmList1 = {0, 1, 2, 3, 4}
elseif MOD_TYPE == "air103" then
    pwmList1 = {00, 01, 02, 03, 04}
    pwmList2 = {10, 11, 12, 13, 14}
    pwmList3 = {20, 21, 22, 23, 24}
    pwmList4 = {30, 31, 32, 33, 34}
elseif MOD_TYPE == "air105" then
    pwmList1 = {0, 1, 2, 3, 4, 5, 6, 7}
end

function pwmTest.test()
    if pwm == nil then
        log.error(tag, "this fireware is not support pwm")
        return
    end
    log.info(tag, "START")
    for _, v in pairs(pwmList1) do
        assert(pwm.open(v, 10000, 99, 0) == true, tag .. ".open ERROR")
        -- log.info(tag .. "capture",pwm.capture(v,1000))
        -- pwm.close(v)
    end
    log.info(tag, "DONE")
end

return pwmTest
