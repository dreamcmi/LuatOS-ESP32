# rmt - esp专用rmt库

> 本页文档由[这个文件](https://gitee.com/openLuat/LuatOS/tree/master/luat/port/luat_rmt_esp32.c)自动生成。如有错误，请提交issue或帮忙修改后pr，谢谢！

## rmt.init(io,ch,div)

初始化rmt

**参数**

|传入值类型|解释|
|-|-|
|int|配置GPIO|
|int|配置rmt通道 0/1|
|int|apb分频参数(APB时钟80Mhz)|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err 成功0|

**例子**

```lua
rmt.init(8,0,1)

```

---

## rmt.deinit(ch)

去初始化rmt

**参数**

|传入值类型|解释|
|-|-|
|int|配置rmt通道 0/1|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err 成功0|

**例子**

```lua
rmt.deinit(0)

```

---

