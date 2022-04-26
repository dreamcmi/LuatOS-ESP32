# espws - websocket客户端

## espws.init(uri,username,password)

初始化一个websocket client

**参数**

|传入值类型|解释|
|-|-|
|string|uri|
|string|用户名|
|string|密码|

**返回值**

|返回值类型|解释|
|-|-|
|userdata|参数ok返回句柄, 否则返回nil|

**例子**

```lua
local ws = espws.init("ws://airtest.openluat.com:2900/websocket")
```

---

## espws.start(handle)

启动websocket

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过espws.init创建的句柄|

**返回值**

|返回值类型|解释|
|-|-|
|bool|成功返回true,否则返回false|

**例子**

```lua
espws.start(ws)
```

---

## espws.close(handle)

关闭websocket

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过espws.init创建的句柄|

**返回值**

|返回值类型|解释|
|-|-|
|bool|成功返回true,否则返回false|

**例子**

```lua
espws.close(ws)
```

---

## espws.destory(handle)

销毁websocket

**参数**

| 传入值类型 | 解释                     |
| ---------- | ------------------------ |
| userdata   | 通过espws.init创建的句柄 |

**返回值**

| 返回值类型 | 解释                       |
| ---------- | -------------------------- |
| bool       | 成功返回true,否则返回false |

**例子**

```lua
espws.destory(ws)
```

## espws.send(handle,data)

websocket发送消息

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过espmqtt.init创建的句柄|
|string|要发送的消息|

**返回值**

|返回值类型|解释|
|-|-|
|bool|成功返回true,否则返回false|

**例子**

```lua
espws.send(ws, "hello luatos esp32")
```

---

