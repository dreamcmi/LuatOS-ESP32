local packTest = {}

local tag = "packTest"

function packTest.test()
    if pack == nil then
        log.error(tag, "this fireware is not support pack")
        return
    end
    log.info(tag, "START")
    local data = pack.pack(">fdncbhHiIlLzpPaA", 3.5, 3.5, 3.5, 1, 2, 3, 4, 100,
                           100, 100, 100, "中国", "中国", "中国",
                           "中国", "ABCD")

    local _, res1, res2, res3, res4, res5, res6, res7, res8, res9, res10, res11,
          res12, res13, res14, res15, res16, res17, res18, res19 = pack.unpack(
                                                                       data,
                                                                       ">fdncbhHiIlLzpPaAAAA")
    local resTable = {
        res1, res2, res3, res4, res5, res6, res7, res8, res9, res10, res11,
        res12, res13, res14, res15, res16, res17, res18, res19
    }
    local assertTable = {
        3.500000, 3.500000, 3.500000, 1, 2, 3, 4, 100, 100, 100, 100, "中国",
        "中国", "中国", "中国", "A", "B", "C", "D"
    }
    for k, v in pairs(resTable) do
        assert(v == assertTable[k], tag .. ".unpack ERROR")
    end

    log.info(tag, "DONE")
end

return packTest
