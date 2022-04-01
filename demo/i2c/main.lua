PROJECT = "i2cdemo"
VERSION = "1.0.0"

-- sys库是标配
local sys = require "sys"

--7bit地址，不包含最后一位读写位
local addr = 0x70
-- 按照实际芯片更改编号哦
local i2cid = 0

-- 示例使用shtc3
sys.taskInit(
    function()
        log.info("i2c.init", i2c.setup(0))
        i2c.send(i2cid, addr, string.char(0x38, 0x17)) -- 唤醒
        -- i2c.send(i2cid, addr, string.char(0xef, 0xc8)) -- 读id
        -- local data = i2c.recv(i2cid, addr, 3)
        -- log.info("recv",data:toHex())
        while true do
            i2c.send(i2cid, addr, string.char(0x64, 0x58)) -- 低功耗 温度在前
            sys.wait(100)
            local data = i2c.recv(i2cid, addr, 6)
            local _, tval, ccrc, hval, hcrc = pack.unpack(data, ">HbHb")
            log.info("shtc3.tem", string.format("%.1fC", ((tval / 65536.0) * 175) - 45))
            log.info("shtc3.hum", string.format("%.1f%%", (hval / 65536.0) * 100))
            sys.wait(1000)
        end
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
