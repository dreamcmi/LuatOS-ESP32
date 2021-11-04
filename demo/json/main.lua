PROJECT = "jsondemo"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

log.info("main", "json demo")
print(json.null)

local t = {
    a = 1,
    b = "abc",
    c = {
        1,2,3,4
    },
    d = {
        x = false,
        j = 111111
    },
    aaaa = 6666,
}

local s = json.encode(t)

local st = json.decode(s)

print(s)
print(st.a,st.b,st.d.x)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
