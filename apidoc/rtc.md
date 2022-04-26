# rtc - 实时时钟

## rtc.set(tab)

设置时钟

**参数**

|传入值类型|解释|
|-|-|
|table|时钟参数,见示例|

**返回值**

|返回值类型|解释|
|-|-|
|bool|成功返回true,否则返回nil或false|

**例子**

```lua
rtc.set({year=2021,mon=8,day=31,hour=17,min=8,sec=43})

```

---

## rtc.get()

获取时钟

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|table|时钟参数,见示例|

**例子**

```lua
local t = rtc.get()
-- {year=2021,mon=8,day=31,hour=17,min=8,sec=43}
log.info("rtc", json.encode(t))

```

---

