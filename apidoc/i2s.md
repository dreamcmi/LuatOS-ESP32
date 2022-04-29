# i2s - I2S操作

## i2s.setup(id,rate,bit,channel_format,communication_format,mclk_multiple)

i2s初始化

**参数**

|传入值类型|解释|
|-|-|
|int|设备id|
|int|i2s采样率 默认16k 可选8/16/24/32|
|int|1个channel对应的位数 默认16bit|
|int|通道设置 默认左右声道 可选i2s.RLCH(左右声道) i2s.ARCH(全右声道) i2s.ALCH(全左声道) i2s.ORCH(单右通道) i2s.OLCH(单左通道)|
|int|i2s通讯格式 默认i2s 可选i2s.STAND_I2S i2s.STAND_MSB i2s.STAND_PCM_SHORT i2s.STAND_PCM_LONG|
|int|mclk频率 默认sample_rate * 256 可选128/256/384|

**返回值**

|返回值类型|解释|
|-|-|
|boolean|成功返回true,否则返回false|

**例子**

```lua
i2s.setup(0,48*1000)
```

## i2s.send(id, buff)

i2s发送数据

**参数**

|传入值类型|解释|
|-|-|
|int|设备id|
|string|待发送的数据|

**返回值**

|返回值类型|解释|
|-|-|
|int|esp_err_t|

**例子**

```lua
i2s.send(0,string.fromHex("ff"))
```

---

## i2s.recv(id, len)

i2s接收数据

**参数**

|传入值类型|解释|
|-|-|
|int|设备id,|
|int|接收数据的长度，默认1024|

**返回值**

|返回值类型|解释|
|-|-|
|string|收到的数据|

**例子**

```lua
local data = i2s.send(0,256)
```

---

## i2s.close(id)

关闭i2c设备

**参数**

|传入值类型|解释|
|-|-|
|int|设备id|

**返回值**

|返回值类型|解释|
|-|-|
|boolean|成功返回true,否则返回false|

**例子**

```lua
i2s.close(0)
```

---

## i2s.mp3playerStart(id,path)

mp3播放

**参数**

|传入值类型|解释|
|-|-|
|int|设备id|
|string|mp3文件路径|

**返回值**

|返回值类型|解释|
|-|-|
|boolean|成功返回true,否则返回false|

**例子**

```lua
i2s.mp3playerStart(0,"spiffs/test.mp3")
```

## i2s.mp3playerPause()

mp3播放暂停

**参数**

无

**返回值**

| 返回值类型 | 解释                       |
| ---------- | -------------------------- |
| boolean    | 成功返回true,否则返回false |

**例子**

```lua
i2s.mp3playerPause()
```

## i2s.mp3playerStop()

mp3播放终止

**参数**

无

**返回值**

| 返回值类型 | 解释                       |
| ---------- | -------------------------- |
| boolean    | 成功返回true,否则返回false |

**例子**

```lua
i2s.mp3playerStop()
```

