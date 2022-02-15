# espmqtt - mqtt/websocket客户端

## espmqtt.init(params)

初始化mqtt/websocket客户端

**参数**

|传入值类型|解释|
|-|-|
|table|参数表,包括: uri,username,password,client_id,keepalive等|

**返回值**

|返回值类型|解释|
|-|-|
|userdata|参数ok返回句柄, 否则返回nil|

**例子**

```lua
-- 创建一个mqttc实例, 详细用法请查阅 demo/espmqtt
local mqttc = espmqtt.init({
    uri = "mqtt://lbsmqtt.airm2m.com:1884",
    client_id = (esp32.getmac():toHex())
})
log.info("mqttc", "client", mqttc)

```

---

## espmqtt.start(client)

启动客户端,连接到服务器

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过espmqtt.init创建的句柄|

**返回值**

|返回值类型|解释|
|-|-|
|bool|成功返回true,否则返回false|

**例子**

无

---

## espmqtt.stop(client)

停止客户端,断开与服务器的连接

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过espmqtt.init创建的句柄|

**返回值**

|返回值类型|解释|
|-|-|
|bool|成功返回true,否则返回false|

**例子**

无

---

## espmqtt.stop(client)

销毁客户端,清理相关资源

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过espmqtt.init创建的句柄|

**返回值**

|返回值类型|解释|
|-|-|
|bool|成功返回true,否则返回false|

**例子**

无

---

## espmqtt.subscribe(client, topic, qos)

订阅一个主题

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过espmqtt.init创建的句柄|
|string|主题, 必须填写|
|int|级别,默认为0,可选|

**返回值**

|返回值类型|解释|
|-|-|
|bool|成功返回true,否则返回false|

**例子**

无

---

## espmqtt.unsubscribe(client, topic)

取消订阅一个主题

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过espmqtt.init创建的句柄|
|string|主题, 必须填写|

**返回值**

|返回值类型|解释|
|-|-|
|bool|成功返回true,否则返回false|

**例子**

无

---

## espmqtt.publish(client, topic, data, qos, retain)

往主题发送一个消息

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过espmqtt.init创建的句柄|
|string|主题, 必须填写|
|string|数据, 必须填写|
|int|级别,默认为0,可选|
|int|默认为0|

**返回值**

|返回值类型|解释|
|-|-|
|bool|成功返回true,否则返回false|

**例子**

无

---

