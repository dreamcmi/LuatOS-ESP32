local zbuffTest = {}

local tag = "zbuffTest"

function zbuffTest.test()
    if zbuff == nil then
        log.error(tag, "this fireware is not support zbuff")
        return
    end
    log.info(tag, "START")
    local buff = zbuff.create(1024)
    assert(type(buff) == "userdata", tag .. ".create ERROR")
    assert(buff:len() == 1024, tag .. ".len ERROR")
    assert(buff:write("ZBUFFTEST") == 9, tag .. ".write ERROR")
    assert(buff:seek(0) == 0, tag .. ".seek ERROR")
    assert(buff:read(9) == "ZBUFFTEST", tag .. ".read ERROR")
    assert(buff:seek(0) == 0, tag .. ".seek ERROR")
    assert(buff:writeU8(0x41) == 1, tag .. ".writeU8 ERROR")
    assert(buff:seek(0) == 0, tag .. ".seek ERROR")
    assert(buff:readU8() == 0x41, tag .. ".readU8 ERROR")
    assert(buff:seek(0) == 0, tag .. ".seek ERROR")
    assert(buff:read(9) == "ABUFFTEST", tag .. ".read ERROR")
    assert(buff:toStr(0, 9) == "ABUFFTEST", tag .. ".toStr ERROR")
    assert(buff[0] == 65, tag .. ".buff[n] ERROR")
    assert(buff[8] == 84, tag .. ".buff[n] ERROR")
    log.info(tag, "DONE")
end

return zbuffTest
