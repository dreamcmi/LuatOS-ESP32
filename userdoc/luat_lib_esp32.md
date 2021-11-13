# esp32.getmac()

```lua
esp32.getmac(id)
```

获取mac

## 参数表

| Name | Type  | Description                                                  |
| ---- | ----- | ------------------------------------------------------------ |
| `id` | `int` | mac地址来源 0:ESP_MAC_WIFI_STA 1:ESP_MAC_WIFI_SOFTAP 2:ESP_MAC_BT 3:ESP_MAC_ETH |

## **返回值**

> `string` mac地址

## **调用示例**

```lua
mac = esp32.getmac(0)
log.info("mac",mac)
```



# esp32.random()

```lua
esp32.random()
```

获取随机数

## 参数表

## **返回值**

> `int` number

## **调用示例**

```lua
ran = esp32.random()
log.info("ran",ran)
```



# esp32.getchip()

```lua
esp32.getchip()
```

获取chip信息

## 参数表

## **返回值**

> `table`

## **调用示例**

```lua
local re = esp32.getchip()F
log.info("esp32", "chip", re['chip'])
log.info("esp32", "features", re['features'])
log.info("esp32", "cores", re['cores'])
log.info("esp32", "revision", re['revision'])
```



# esp32.getRstReason()

```lua
esp32.getRstReason()
```

获取重启原因

## 参数表

## **返回值**

> `int`
>
> ```c
> typedef enum {
>     ESP_RST_UNKNOWN,    //!< Reset reason can not be determined
>     ESP_RST_POWERON,    //!< Reset due to power-on event
>     ESP_RST_EXT,        //!< Reset by external pin (not applicable for ESP32)
>     ESP_RST_SW,         //!< Software reset via esp_restart
>     ESP_RST_PANIC,      //!< Software reset due to exception/panic
>     ESP_RST_INT_WDT,    //!< Reset (software or hardware) due to interrupt watchdog
>     ESP_RST_TASK_WDT,   //!< Reset due to task watchdog
>     ESP_RST_WDT,        //!< Reset due to other watchdogs
>     ESP_RST_DEEPSLEEP,  //!< Reset after exiting deep sleep mode
>     ESP_RST_BROWNOUT,   //!< Brownout reset (software or hardware)
>     ESP_RST_SDIO,       //!< Reset over SDIO
> } esp_reset_reason_t;
> ```
>
> 

## **调用示例**

```lua
reason =  esp32.getRstReason()
log.info("reason",reason)
```



# esp32.getWakeupCause()

```lua
esp32.getWakeupCause()
```

获取唤醒原因

## 参数表

## **返回值**

> `int`
>
> ```c
> typedef enum {
>     ESP_SLEEP_WAKEUP_UNDEFINED,    //!< In case of deep sleep, reset was not caused by exit from deep sleep
>     ESP_SLEEP_WAKEUP_ALL,          //!< Not a wakeup cause, used to disable all wakeup sources with esp_sleep_disable_wakeup_source
>     ESP_SLEEP_WAKEUP_EXT0,         //!< Wakeup caused by external signal using RTC_IO
>     ESP_SLEEP_WAKEUP_EXT1,         //!< Wakeup caused by external signal using RTC_CNTL
>     ESP_SLEEP_WAKEUP_TIMER,        //!< Wakeup caused by timer
>     ESP_SLEEP_WAKEUP_TOUCHPAD,     //!< Wakeup caused by touchpad
>     ESP_SLEEP_WAKEUP_ULP,          //!< Wakeup caused by ULP program
>     ESP_SLEEP_WAKEUP_GPIO,         //!< Wakeup caused by GPIO (light sleep only)
>     ESP_SLEEP_WAKEUP_UART,         //!< Wakeup caused by UART (light sleep only)
>     ESP_SLEEP_WAKEUP_WIFI,              //!< Wakeup caused by WIFI (light sleep only)
>     ESP_SLEEP_WAKEUP_COCPU,             //!< Wakeup caused by COCPU int
>     ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG,   //!< Wakeup caused by COCPU crash
>     ESP_SLEEP_WAKEUP_BT,           //!< Wakeup caused by BT (light sleep only)
> } esp_sleep_source_t;
> ```
>
> 

## **调用示例**

```lua
cause = esp32.getWakeupCause()
log.info("cause",cause)
```



# esp32.enterLightSleep(waketype,pin/rtc,level/nil)

```lua
esp32.enterLightSleep(waketype,pin/rtc,level/nil)esp32.getchip()
```

lightsleep

## 参数表

| Name        | Type  | Description                                        |
| ----------- | ----- | -------------------------------------------------- |
| `type`      | `int` | 唤醒类型 gpio(0) rtc(1)                            |
| `condition` | `int` | 唤醒条件 gpio(pin) rtc(time:us)                    |
| `level`     | `int` | gpio唤醒沿 0:下降沿 1:上升沿 (rtc唤醒不需要此参数) |

## **返回值**

> 无

## **调用示例**

```lua
-- gpio9唤醒
esp32.enterLightSleep(0,9,0)
-- rtc唤醒10s
esp32.enterLightSleep(1,10*1000*1000)
```



# esp32.enterDeepSleep(waketype,pin/rtc,level/nil)

```lua
esp32.enterDeepSleep(waketype,pin/rtc,level/nil)
```

lightsleep

## 参数表

| Name        | Type  | Description                                        |
| ----------- | ----- | -------------------------------------------------- |
| `type`      | `int` | 唤醒类型 gpio(0) rtc(1)                            |
| `condition` | `int` | 唤醒条件 gpio(pin) rtc(time:us)                    |
| `level`     | `int` | gpio唤醒沿 0:下降沿 1:上升沿 (rtc唤醒不需要此参数) |

## **返回值**

> 无

## **调用示例**

```lua
-- gpio唤醒
esp32.enterDeepSleep(0,5,0)
-- rtc唤醒1
esp32.enterDeepSleep(1,10*1000*1000)
```

