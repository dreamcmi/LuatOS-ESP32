# wlan - esp32_wifi操作库

## wlan.getMode()

获取wifi模式

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|int|模式wlan.NONE, wlan.STATION, wlan.AP,wlan.STATIONAP|

**例子**

```lua
-- 获取wifi的当前模式
local m = wlan.getMode()

```

---

## wlan.setMode(mode)

设置wifi模式

**参数**

|传入值类型|解释|
|-|-|
|int|模式wlan.NONE, wlan.STATION, wlan.AP,wlan.STATIONAP|

**返回值**

|返回值类型|解释|
|-|-|
|int|返回esp_err|

**例子**

```lua
-- 将wlan设置为wifi客户端模式
wlan.setMode(wlan.STATION)

```

---

## wlan.init()

初始化wifi

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
-- 在使用wifi前初始化一下
wlan.init()

```

---

## wlan.connect(ssid,password,autoreconnect)

连接wifi,成功启动联网线程不等于联网成功!!

**参数**

|传入值类型|解释|
|-|-|
|string|ssid  wifi的SSID|
|string|password wifi的密码,可选|
|int|断连自动重连 1:启用 0:不启用|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
-- 连接到uiot,密码1234567890
wlan.connect("uiot", "1234567890")

```

---

## wlan.createAP(ssid,password)

创建ap

**参数**

|传入值类型|解释|
|-|-|
|string|ssid  wifi的SSID|
|string|password wifi的密码|
|int|channle 信道 默认11|
|int|最大连接数 默认5|
|int|authmode 密码验证模式|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
wlan.createAP("LuatOS-ESP32","12345678")

```

---

## wlan.disconnect()

断开wifi

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
-- 断开wifi连接
wlan.disconnect()

```

---

## wlan.deinit()

去初始化wifi

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
-- 去初始化wifi
wlan.deinit()

```

---

## wlan.setps(ID)

设置wifi省电

**参数**

|传入值类型|解释|
|-|-|
|int|省电等级 省电等级 wlan.PS_NONE  wlan.PS_MIN_MODEM wlan.PS_MAX_MODEM|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
wlan.setps(1)

```

---

## wlan.getps()

获取wifi省电模式

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
wlan.getps()

```

---

## wlan.smartconfig()

smartconfig配网(默认esptouch)

**参数**

|传入值类型|解释|
|-|-|
|int|mode 0:ESPTouch 1:AirKiss 2:ESPTouch and AirKiss 3:ESPTouch v2|

**返回值**

|返回值类型|解释|
|-|-|
|int|创建成功0 失败1|

**例子**

```lua
wlan.smartconfigStop()

```

---

## wlan.smartconfigStop()

smartconfig配网停止

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
wlan.smartconfig()

```

---

## wlan.dhcp(mode)

wlan dhcp开关

**参数**

|传入值类型|解释|
|-|-|
|int|0:关闭dhcp 1:开启dhcp|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
wlan.dhcp(0) -- 关闭dhcp

```

---

## wlan.setIp(ip,gw,netmask)

wlan设置ip信息

**参数**

|传入值类型|解释|
|-|-|
|string|ip ip地址 格式"xxx.xxx.xxx.xxx"|
|string|gw 网关地址 格式"xxx.xxx.xxx.xxx"|
|string|netmask 子网掩码 格式"xxx.xxx.xxx.xxx"|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
wlan.setIp("192.168.55.1", "192.168.55.1", "255.255.255.0")

```

---

## wlan.setHostname(name)

wlan设置hostname

**参数**

|传入值类型|解释|
|-|-|
|string|hosetname 主机名|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err|

**例子**

```lua
wlan.setHostname("luatos")

```

---

