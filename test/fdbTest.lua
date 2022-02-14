local fdbTest = {}

local tag = "fdbTest"

function fdbTest.test()
    if fdb == nil then
        log.error(tag, "this fireware is not support fdb")
        return
    end
    log.info(tag, "START")
    assert(fdb.kvdb_init("env", "onchip_flash") == true)
    assert(fdb.kv_set("testBool", false) == true, tag .. ".kv_set ERROR")
    assert(fdb.kv_set("testNumber1", 123) == true, tag .. ".kv_set ERROR")
    assert(fdb.kv_set("testNumber2", 1.23) == true, tag .. ".kv_set ERROR")
    assert(fdb.kv_set("testString", "LuatOS-SoC-Test") == true,
           tag .. ".kv_set ERROR")
    assert(
        fdb.kv_set("testTable", {project = "LuatOS-SoC-Test", version = 1.0}) ==
            true, tag .. ".kv_set ERROR")

    assert(fdb.kv_get("testBool") == false, tag .. ".kv_get ERROR")
    assert(fdb.kv_get("testNumber1") == 123, tag .. ".kv_get ERROR")
    assert(fdb.kv_get("testNumber2") == 1.23, tag .. ".kv_get ERROR")
    assert(fdb.kv_get("testString") == "LuatOS-SoC-Test", tag .. ".kv_get ERROR")
    assert(fdb.kv_get("testTable")["project"] == "LuatOS-SoC-Test",
           tag .. ".kv_get ERROR")
    assert(fdb.kv_get("testTable")["version"] == 1.0, tag .. ".kv_get ERROR")
    assert(fdb.kv_del("testBool") == true, tag .. ".kv_del ERROR")
    assert(fdb.kv_get("testBool") == nil, tag .. ".kv_get ERROR")

    log.info(tag, "DONE")
end

return fdbTest
