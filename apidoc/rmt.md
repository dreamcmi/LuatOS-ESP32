# rmt - esp专用rmt库

## rmt.init(io,ch,div)

初始化rmt

**参数**

|传入值类型|解释|
|-|-|
|int|配置GPIO|
|int|配置rmt通道 0/1|
|int|apb分频参数(APB时钟80Mhz)|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err 成功0|

**例子**

```lua
rmt.init(8,0,1)

```

---

## rmt.deinit(ch)

去初始化rmt

**参数**

|传入值类型|解释|
|-|-|
|int|配置rmt通道 0/1|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err 成功0|

**例子**

```lua
rmt.deinit(0)

```

---

## rmt.ws2812Init(num,channel)

初始化ws2812

**参数**

| 传入值类型 | 解释                               |
| ---------- | ---------------------------------- |
| int        | 灯珠数量                           |
| int        | channel rmt tx通道,需要先初始化rmt |

**返回值**

| 返回值类型 | 解释                           |
| ---------- | ------------------------------ |
| userdata   | 如果成功返回handle,失败返回nil |

**例子**

```lua
handle = rmt.ws2812Init(10, 0)
assert(handle ~= nil, "ws2812.init error")
```

---

## rmt.ws2812Clear(handle)

ws2812清空显示

**参数**

| 传入值类型 | 解释                           |
| ---------- | ------------------------------ |
| userdata   | 通过rmt.ws2812Init返回的handle |
| int        | 操作超时时间,可选,默认1000ms   |

**返回值**

| 返回值类型 | 解释          |
| ---------- | ------------- |
| esp_err_t  | esp_err 成功0 |

**例子**

```lua
log.info("ws2812.clear", rmt.ws2812Clear(handle))
```

---

## rmt.ws2812SetPixel(handle, num, r, g, b)

ws2812设置颜色

**参数**

| 传入值类型 | 解释                           |
| ---------- | ------------------------------ |
| userdata   | 通过rmt.ws2812Init返回的handle |
| int        | 要操作的灯珠编号,注意从0开始   |
| int        | red 0-255                      |
| int        | green 0-255                    |
| int        | blue 0-255                     |

**返回值**

| 返回值类型 | 解释          |
| ---------- | ------------- |
| esp_err_t  | esp_err 成功0 |

**例子**

```lua
log.info("ws2812.setpixel", rmt.ws2812SetPixel(handle, 0, 255, 0, 0))
```

---

## rmt.ws2812Refresh(handle)

ws2812刷新

**参数**

| 传入值类型 | 解释                           |
| ---------- | ------------------------------ |
| userdata   | 通过rmt.ws2812Init返回的handle |
| int        | 操作超时时间,可选,默认1000ms   |

**返回值**

| 返回值类型 | 解释          |
| ---------- | ------------- |
| esp_err_t  | esp_err 成功0 |

**例子**

```lua
log.info("ws2812.refresh", rmt.ws2812Refresh(handle))
```

---

## rmt.ws2812Deinit(handle)

ws2812去初始化

**参数**

| 传入值类型 | 解释                           |
| ---------- | ------------------------------ |
| userdata   | 通过rmt.ws2812Init返回的handle |

**返回值**

| 返回值类型 | 解释          |
| ---------- | ------------- |
| esp_err_t  | esp_err 成功0 |

**例子**

```lua
log.info("ws2812.deinit", rmt.ws2812Deinit(handle))
```

---
