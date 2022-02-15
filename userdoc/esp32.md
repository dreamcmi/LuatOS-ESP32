# esp32 - esp32专用库

## esp32.getmac(id)

获取mac

**参数**

|传入值类型|解释|
|-|-|
|int|mac地址来源 0:ESP_MAC_WIFI_STA 1:ESP_MAC_WIFI_SOFTAP 2:ESP_MAC_BT 3:ESP_MAC_ETH|

**返回值**

|返回值类型|解释|
|-|-|
|string|mac地址|

**例子**

```lua
esp32.getmac(0)

```

---

## esp32.getRstReason(id)

获取重启原因

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_reset_reason_t|

**例子**

```lua
esp32.getRstReason()

```

---

## esp32.random()

获取随机数

**参数**

无

**返回值**

| 返回值类型 | 解释   |
| ---------- | ------ |
| int        | 随机数 |

**例子**

```lua
r = esp32.random()

```

---

## esp32.getchip()

获取chip信息

**参数**

无

**返回值**

| 返回值类型 | 解释       |
| ---------- | ---------- |
| table      | chip信息表 |

**例子**

```lua
local re = esp32.getchip()
log.info("esp32", "chip", re['chip'])
log.info("esp32", "features", re['features'])
log.info("esp32", "cores", re['cores'])
log.info("esp32", "revision", re['revision'])

```

---

## esp32.getWakeupCause()

获取唤醒原因

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_sleep_wakeup_cause_t|

**例子**

```lua
cause = esp32.getWakeupCause()

```

---

## esp32.enterLightSleep(waketype,pin/rtc,level/nil)

lightsleep

**参数**

|传入值类型|解释|
|-|-|
|int|唤醒类型 gpio(0) rtc(1)|
|int|唤醒条件 gpio(pin) rtc(time:us)|
|int|gpio唤醒沿 0:下降沿 1:上升沿|

**返回值**

无

**例子**

```lua
-- gpio唤醒
esp32.enterLightSleep(0,9,0)
-- rtc唤醒
esp32.enterLightSleep(1,10*1000*1000)

```

---

## esp32.enterDeepSleep(waketype,pin/rtc,level/nil)

deepsleep

**参数**

|传入值类型|解释|
|-|-|
|int|唤醒类型 gpio(0) rtc(1)|
|int|唤醒条件 gpio(pin) rtc(time:us)|
|int|gpio唤醒沿 0:下降沿 1:上升沿|

**返回值**

无

**例子**

```lua
-- gpio唤醒
esp32.enterDeepSleep(0,9,0)
-- rtc唤醒
esp32.enterDeepSleep(1,10*1000*1000)

```

---

## esp32.temp()

esp32c3芯片温度

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|float|temp |

**例子**

```lua
log.info("esp32","temp",esp32.temp())

```

---

