# twai - esp专用库twai

## twai.setup(brp,seg1,seg2,sjw,accept_code,accept_mask,mode)

初始化twai

**参数**

|传入值类型|解释|
|-|-|
|int|brq 分频参数|
|int|seg1 时序段1|
|int|seg2 时序段2|
|int|sjw 重新同步跳跃宽度|
|int|accept_code 过滤器接收符 32bit|
|int|accept_mask 过滤器接收掩码 32bit|
|int|mode 默认TWAI_MODE_NORMAL 可选(normal:0,no_ack:1,listen_only:2)|
|return|bool|

**返回值**

无

**例子**

```lua
twai.setup(4,15,4,3,0,0xffffffff)

```

---

## twai.close

关闭twai

**参数**

|传入值类型|解释|
|-|-|
|return|bool|

**返回值**

无

**例子**

```lua
twai.close()

```

---

## twai.send(id,data)

twai发送数据

**参数**

|传入值类型|解释|
|-|-|
|int|id |
|string|data|
|return|bool|

**返回值**

无

**例子**

```lua
twai.send(0x12,string.Fromhex("1234"))

```

---

## twai.recv(id,len)

twai接收数据

**参数**

|传入值类型|解释|
|-|-|
|int|id |
|int|len 接收数据长度|
|return|bool|

**返回值**

无

**例子**

```lua
twai.recv(0x12,8)

```

---

## twai.getAlerts()

twai获取警报

**参数**

|传入值类型|解释|
|-|-|
|return|int |

**返回值**

无

**例子**

```lua
local arg = twai.getAlerts()

```

---

## twai.reconfigureAlerts(mask)

twai重新配置启用警报

**参数**

|传入值类型|解释|
|-|-|
|int|mask|
|return|bool|

**返回值**

无

**例子**

```lua
twai.reconfigureAlerts(0x00002000)

```

---

## twai.busRecovery()

twai总线恢复

**参数**

|传入值类型|解释|
|-|-|
|return|bool|

**返回值**

无

**例子**

```lua
twai.busRecovery()

```

---

