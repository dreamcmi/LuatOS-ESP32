# pwm2 - esp32专用pwm2库

## pwm2.init(pwm_timer,pwm_channel,pwm_pin,pwm_bit,pwm_period,pwm_fade)

pwm2初始化

**参数**

|传入值类型|解释|
|-|-|
|int|pwm_timer   定时器id 取值0~3|
|int|pwm_channel 通道id 取值0~5|
|int|pwm_pin     引脚|
|int|pwm_bit     分辨率 取值1~13|
|int|pwm_period  频率|
|int|pwm_fade    渐入渐出服务|

**返回值**

|返回值类型|解释|
|-|-|
|int|pwm_channel|

**例子**

无

---

## pwm2.setDuty(pwm_channel,pwm_duty)

pwm2调节占空比

**参数**

|传入值类型|解释|
|-|-|
|int|pwm_channel 通道id 取值0~5|
|int|pwm_duty    占空比|
|return|esp_err_t|

**返回值**

无

**例子**

```lua
pwm2.setDuty(0,512)

```

---

## pwm2.setFadeWithTime(pwm_channel,pwm_duty,pwm_ms,pwm_wait)

pwm2渐入渐出定时

**参数**

|传入值类型|解释|
|-|-|
|int|pwm_channel 通道id 取值0~5|
|int|pwm_duty    占空比|
|int|pwm_ms      到达目标占空比的时间(ms)|
|int|pwm_wait    默认为1(阻塞)|
|return|esp_err_t|

**返回值**

无

**例子**

```lua
pwm2.setFadeWithTime(0,512,10)

```

---

## pwm2.getFreq(pwm_timer)

pwm2获取定时器频率

**参数**

|传入值类型|解释|
|-|-|
|int|pwm_timer 定时器|

**返回值**

|返回值类型|解释|
|-|-|
|int|频率|

**例子**

```lua
freq = pwm2.getFreq(0)

```

---

## pwm2.uninstallFade()

pwm2卸载渐入渐出服务

**参数**

无

**返回值**

无

**例子**

```lua
pwm2.uninstallFade()

```

---

