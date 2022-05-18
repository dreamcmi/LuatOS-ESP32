# wlan - esp32_wifi操作库

## wlan.init()

初始化wifi

**参数**

无

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
-- 在使用wifi前初始化一下
wlan.init()

```

---

## wlan.getMode()

获取wifi模式

**参数**

无

**返回值**

| 返回值类型 | 解释                                                |
| ---------- | --------------------------------------------------- |
| int        | 模式wlan.NONE, wlan.STATION, wlan.AP,wlan.STATIONAP |

**例子**

```lua
-- 获取wifi的当前模式
local m = wlan.getMode()

```

---

## wlan.setMode(mode)

设置wifi模式

**参数**

| 传入值类型 | 解释                                                |
| ---------- | --------------------------------------------------- |
| int        | 模式wlan.NONE, wlan.STATION, wlan.AP,wlan.STATIONAP |

**返回值**

| 返回值类型 | 解释        |
| ---------- | ----------- |
| int        | 返回esp_err |

**例子**

```lua
-- 将wlan设置为wifi客户端模式
wlan.setMode(wlan.STATION)

```

---



## wlan.connect(ssid,password,autoreconnect)

连接wifi,成功启动联网线程不等于联网成功!!

**参数**

| 传入值类型 | 解释                         |
| ---------- | ---------------------------- |
| string     | ssid  wifi的SSID             |
| string     | password wifi的密码,可选     |
| int        | 断连自动重连 1:启用 0:不启用 |

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
-- 连接到uiot,密码1234567890
wlan.connect("uiot", "1234567890")

```

---

## wlan.createAP(ssid,password)

创建ap

**参数**

| 传入值类型 | 解释                  |
| ---------- | --------------------- |
| string     | ssid  wifi的SSID      |
| string     | password wifi的密码   |
| int        | channle 信道 默认11   |
| int        | 最大连接数 默认5      |
| int        | authmode 密码验证模式 |

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

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

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
-- 断开wifi连接
wlan.disconnect()

```

---

## wlan.stop()

关闭wifi

**参数**

无

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
-- 关闭wifi
wlan.stop()

```

## wlan.ready()

wifi是否已获取ip

**参数**

无

**返回值**

| 返回值类型 | 解释                           |
| ---------- | ------------------------------ |
| boolean    | 已经有ip返回true,否则返回false |

**例子**

```lua
-- 查询是否已经wifi联网
if wlan.ready() then
    log.info("wlan", "wifi ok", "Let's Rock!")
end
```

---

## wlan.deinit()

去初始化wifi

**参数**

无

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
-- 去初始化wifi
wlan.deinit()

```

---

## wlan.setps(ID)

设置wifi省电

**参数**

| 传入值类型 | 解释                                                         |
| ---------- | ------------------------------------------------------------ |
| int        | 省电等级 省电等级 wlan.PS_NONE  wlan.PS_MIN_MODEM wlan.PS_MAX_MODEM |

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
wlan.setps(wlan.PS_MAX_MODEM)

```

---

## wlan.getps()

获取wifi省电模式

**参数**

无

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
wlan.getps()

```

---

## wlan.setProtocol(mode,protocaolmap)

设置wifi协议

**参数**

| 传入值类型 | 解释                                               |
| ---------- | -------------------------------------------------- |
| int        | mode wlan.IF_STA wlan.IF_AP                        |
| int        | protocaolmap wlan.P11B wlan.P11G wlan.P11N wlan.LR |

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
wlan.setProtocol(wlan.IF_STA , wlan.P11B | wlan.P11G)
```

---

## wlan.getProtocol(mode)

获取wifi协议

**参数**

| 传入值类型 | 解释                        |
| ---------- | --------------------------- |
| int        | mode wlan.IF_STA wlan.IF_AP |

**返回值**

| 返回值类型 | 解释         |
| ---------- | ------------ |
| int        | protocaolmap |

**例子**

```lua
log.info("wlan.protocol",wlan.getProtocol(wlan.IF_STA)) 
```

---

## wlan.setBandwidth(mode,bw)

设置wifi带宽

**参数**

| 传入值类型 | 解释                        |
| ---------- | --------------------------- |
| int        | mode wlan.IF_STA wlan.IF_AP |
| int        | bw wlan.HT20 wlan.HT40      |

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
wlan.setBandwidth(wlan.IF_STA,wlan.HT20)
```

---

## wlan.getBandwidth(mode)

获取wifi带宽

**参数**

| 传入值类型 | 解释                        |
| ---------- | --------------------------- |
| int        | mode wlan.IF_STA wlan.IF_AP |

**返回值**

| 返回值类型 | 解释                        |
| ---------- | --------------------------- |
| int        | mode wlan.IF_STA wlan.IF_AP |

**例子**

```lua
log.info("wlan.bw",wlan.getBandwidth(wlan.IF_STA))
```

---


## wlan.smartconfig(mode)

smartconfig配网(默认esptouch)

**参数**

| 传入值类型 | 解释                                                         |
| ---------- | ------------------------------------------------------------ |
| int        | mode 0:ESPTouch 1:AirKiss 2:ESPTouch and AirKiss 3:ESPTouch v2 |

**返回值**

| 返回值类型 | 解释            |
| ---------- | --------------- |
| int        | 创建成功0 失败1 |

**例子**

```lua
wlan.smartconfig()

```

---

## wlan.dhcp(mode)

wlan dhcp开关

**参数**

| 传入值类型 | 解释                  |
| ---------- | --------------------- |
| int        | 0:关闭dhcp 1:开启dhcp |

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
wlan.dhcp(0) -- 关闭dhcp

```

---

## wlan.setIp(ip,gw,netmask)

wlan设置ip信息

**参数**

| 传入值类型 | 解释                                   |
| ---------- | -------------------------------------- |
| string     | ip ip地址 格式"xxx.xxx.xxx.xxx"        |
| string     | gw 网关地址 格式"xxx.xxx.xxx.xxx"      |
| string     | netmask 子网掩码 格式"xxx.xxx.xxx.xxx" |

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
wlan.setIp("192.168.55.1", "192.168.55.1", "255.255.255.0")

```

---

## wlan.setHostname(name)

wlan设置hostname

**参数**

| 传入值类型 | 解释             |
| ---------- | ---------------- |
| string     | hosetname 主机名 |

**返回值**

| 返回值类型 | 解释    |
| ---------- | ------- |
| int        | esp_err |

**例子**

```lua
wlan.setHostname("luatos")

```

---

## wlan.getConfig(mode)

wlan获取配置信息

**参数**

| 传入值类型 | 解释            |
| ---------- | --------------- |
| int        | mode STA:0 AP:1 |

**返回值**

| 返回值类型 | 解释                                                         |
| ---------- | ------------------------------------------------------------ |
| table      | STA:{"ssid":"xxx","password":"xxx","bssid":"xxx"} AP:{"ssid":"xxx","password":"xxx"} |

**例子**

```lua
-- AP
t = wlan.getConfig(1)
log.info("wlan", "wifi ap info", t.ssid, t.password)
-- STA
t = wlan.getConfig(0)
log.info("wlan", "wifi connected info", t.ssid, t.password, t.bssid:toHex())

```

## wlan.scan()

开始扫网,通常配合wlan.scanResult使用

**参数**

无

**返回值**

| 返回值类型 | 解释                |
| ---------- | ------------------- |
| boolean    | 启动结果,一般为true |

**例子**

```lua
wlan.scan()
sys.waitUntil("WLAN_SCAN_DONE", 30000)
local re = wlan.scanResult()
for i in ipairs(re) do
    log.info("wlan", "info", re[i].ssid, re[i].rssi)
end
```

---

## wlan.scanResult(num)

获取扫网结果,需要先执行wlan.scan,并等待WLAN_SCAN_DONE事件

**参数**

| 输入值类型 | 解释                |
| ---------- | ------------------- |
| int        | 最大结果数量,默认50 |

**返回值**

| 返回值类型 | 解释           |
| ---------- | -------------- |
| table      | 扫描结果的数组 |

**例子**

```lua
-- 扫描并查询结果
wlan.scan()
sys.waitUntil("WLAN_SCAN_DONE", 30000)
local re = wlan.scanResult()
for i in ipairs(re) do
    log.info("wlan", "info", re[i].ssid, re[i].rssi)
end
```

---

## wlan.setMac(mode,mac)

wlan获取配置信息

**参数**

| 传入值类型 | 解释            |
| ---------- | --------------- |
| int        | mode STA:0 AP:1 |
| string     | 6位mac地址      |

**返回值**

| 返回值类型 | 解释      |
| ---------- | --------- |
| int        | esp_err_t |

**例子**

```lua
wlan.setMac(1, string.fromHex("60AAF973090E"))
```







