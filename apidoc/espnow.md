# espnow - espnow库

## espnow.init()

初始化espnow

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|int|0成功  |

**例子**

```lua
espnow.init()

```

---

## espnow.setPmk(pmk)

设置pmk

**参数**

|传入值类型|解释|
|-|-|
|string|pmk primary master key|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
espnow.setPmk("pmk1234567890123")

```

---

## espnow.addPeer(mac,lmk)

增加espnow peer

**参数**

|传入值类型|解释|
|-|-|
|string|mac地址|
|string|lmk local master key|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
espnow.addPeer(string.fromHex("0016EAAE3C40"),"lmk1234567890123")

```

---

## espnow.send(mac,data)

espnow发送

**参数**

|传入值类型|解释|
|-|-|
|string|mac地址|
|string|发送的数据|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
espnow.send(string.fromHex("0016EAAE3C40"),"espnow")

```

---

## espnow.deinit()

去初始化espnow

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
espnow.deinit()

```

---

