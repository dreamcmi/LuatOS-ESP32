# sdmmc - esp32_sdmmc操作库

## sdmmc.init(type,freq,width(cs),clk,cmd,d0(,d1,d2,d3))

初始化SD卡与FAT文件系统 默认使用SDSPI 20Mhz

**参数**

|传入值类型|解释|
|-|-|
|int|模式 1=sdio,2=spi C3仅支持SDSPI模式，S3两种均可使用|
|int|频率 SDMMC_FREQ_DEFAULT 20M，SDMMC_FREQ_HIGHSPEED 40M，|
|int|宽度 只支持1bit和4bit 仅适用于sdio模式 在sdspi模式下用于设置cs引脚|
|int|clk引脚号|
|int|cmd引脚号|
|int|d0引脚号|
|int|d1引脚号 当宽度为4bit可用|
|int|d2引脚号 当宽度为4bit可用|
|int|d3引脚号 当宽度为4bit可用|

**返回值**

|返回值类型|解释|
|-|-|
|boolean|成功true 失败false|

**例子**

```lua
-- 使用sdio挂载sd卡
sdmmc.init(1,sdmmc.SDMMC_FREQ_DEFAULT,1,13,12,11)
-- 使用spi挂载sd卡
sdmmc.init(2,sdmmc.SDMMC_FREQ_DEFAULT,4)

```

---

## sdmmc.deinit()

卸载sd卡文件系统

**参数**

无

**返回值**

|返回值类型|解释|
|-|-|
|boolean|成功true 失败false|

**例子**

```lua
-- 卸载sd卡
sdmmc.deinit()

```

---

