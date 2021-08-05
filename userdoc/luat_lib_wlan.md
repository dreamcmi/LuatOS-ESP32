# wlan.init()

```
wlan.init()
```

初始化wifi

## 调用示例

```
-- 在使用wifi前初始化一下
wlan.init()
```

# wlan.getMode

```lua
wlan.getMode(dev)
```

获取wifi模式

## 参数表

Name | Type | Description
-----|------|--------------
`dev`|`string`| 设备名称,字符串或数值, 可选值0/1, "wlan0","wlan1". 默认"wlan0"

## 返回值

> `int`: 模式wlan.NONE, wlan.STATION, wlan.AP

## 调用示例

```lua
-- 获取wlan0的当前模式
local m = wlan.getMode("wlan0")
```


--------------------------------------------------
# wlan.setMode

```lua
wlan.setMode(dev, mode)
```

设置wifi模式,通常不需要设置

## 参数表

Name | Type | Description
-----|------|--------------
`dev`|`string`| 设备名称,字符串或数值, 可选值0/1, "wlan0","wlan1". 默认"wlan0"
`mode`|`int`| 模式wlan.NONE, wlan.STATION, wlan.AP

## 返回值

> `int`: 设置成功与否,通常不检查

## 调用示例

```lua
-- 将wlan设置为wifi客户端模式
wlan.setMode("wlan0",wlan.STATION) 
```


--------------------------------------------------
# wlan.connect

```lua
wlan.connect(ssid, password)
```

连接wifi,成功启动联网线程不等于联网成功!!

## 参数表

Name | Type | Description
-----|------|--------------
`ssid`|`string`| ssid  wifi的SSID
`password`|`string`| password wifi的密码,可选

## 返回值

> `boolean`: 如果正常启动联网线程,无返回值,否则返回出错信息.

## 调用示例

```lua
-- 连接到uiot,密码1234567890
wlan.connect("uiot", "1234567890")
```


--------------------------------------------------
# wlan.disconnect

```lua
wlan.disconnect()
```

断开wifi

## 参数表

> 无参数

## 返回值

> `boolean`: 成功返回true,否则返回false

## 调用示例

```lua
-- 断开wifi连接
wlan.disconnect()
```


--------------------------------------------------
