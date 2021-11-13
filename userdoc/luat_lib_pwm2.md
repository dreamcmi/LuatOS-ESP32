# pwm2.init

```lua
pwm2.init(pwm_timer,pwm_channel,pwm_pin,pwm_bit,pwm_period,pwm_fade)
```

pwm2初始化

## 参数表

Name | Type | Description
-----|------|--------------
`pwm_timer`|`int`| PWM定时器id（0~3） 
`pwm_channel`|`int`| PWM通道id（0~5） 
`pwm_pin`|`int`| 绑定引脚 
`pwm_bit`|`int`| 分辨率（1~13） 
`pwm_period`|`int`| 频率 
`pwm_fade`|`int`| 渐入渐出服务（开启1 关闭0） 

## 返回值

`int`返回pwm_channel

## 调用示例

```lua
pwm2.init(0,0,1,10,1*1000,0)
```


--------------------------------------------------
# pwm2.setDuty

```lua
pwm2.setDuty(pwm_channel,pwm_duty)
```

pwm2调节占空比

## 参数表

Name | Type | Description
-----|------|--------------
`pwm_channel`|`int`| PWM通道
`pwm_duty`|`int`| 占空比 

## 返回值

> `int` 

## 调用示例

```lua
pwm2.setDuty(0,512)
```

# pwm2.getFreq

```lua
pwm2.getFreq(pwm_timer)
```

pwm2获取定时器频率

## 参数表

| Name        | Type  | Description |
| ----------- | ----- | ----------- |
| `pwm_timer` | `int` | PWM定时器   |

## 返回值

> `int`  freq

## 调用示例

```lua
freq = pwm2.getFreq(0)
```

# pwm2.uninstallFade()

```lua
pwm2.uninstallFade()
```

pwm2卸载渐入渐出服务

## 参数表

## 返回值

> 无

## 调用示例

```lua
pwm2.uninstallFade()
```

