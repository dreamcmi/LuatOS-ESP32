local jsonTest = {}

local tag = "jsonTest"

function jsonTest.test()
    if json == nil then
        log.error(tag, "this fireware is not support json")
        return
    end
    log.info(tag, "START")
    local testable = {
        a = 1,
        b = "abc",
        c = {1, 2, 3, 4},
        d = {x = false, j = 111111},
        aaaa = 6666
    }

    local restring = json.encode(testable)
    local restable = json.decode(restring)

    assert(type(restring) == "string", tag .. ".encode ERROR")
    assert(
        restable.a == 1.000000 and restable.b == "abc" and type(restable.c) ==
            "table" and restable.d.x == false and restable.aaaa == 6666.000,
        tag .. ".decode ERROR")
    log.info(tag, "DONE")
end

return jsonTest
