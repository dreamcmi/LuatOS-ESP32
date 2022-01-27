local fsTest = {}

local tag = "fsTest"

function fsTest.test()
    if fs == nil then
        log.error(tag, "this fireware is not support fs")
        return
    end
    log.info(tag, "START")
    log.info(tag .. ".fsstat", fs.fsstat("/luadb/"))
    log.info(tag .. ".fsize", fs.fsize("/luadb/main.luac"))
    log.info(tag, "DONE")
end

return fsTest
