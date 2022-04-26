# esphttp - http客户端

## esphttp.init(method, url)

初始化一个URL请求

**参数**

|传入值类型|解释|
|-|-|
|string|请求的方法, 例如 GET/POST/PUT|
|string|网址|

**返回值**

|返回值类型|解释|
|-|-|
|userdata|如果成功就返回client指针,否则返回nil|

**例子**

```lua

-- 异步写法
local httpc = esphttp.init(esphttp.GET, "http://httpbin.org/get")
if httpc then
    local ok, err = esphttp.perform(httpc, true)
    if ok then
        while 1 do
            local result, c, ret, data = sys.waitUntil("ESPHTTP_EVT", 20000)
            if c == httpc then
                if ret == esphttp.EVENT_ERROR or ret == esphttp.EVENT_DISCONNECTED then
                    break
                end
                if ret == esphttp.EVENT_ON_DATA and esphttp.status_code(httpc) == 200 then
                    log.info("data", "resp", data)
                end
            end
        end
    else
        log.warn("esphttp", "bad perform", err)
    end
    esphttp.cleanup(httpc)
end

```

---

## esphttp.post_field(client, data)

设置请求的body

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过esphttp.init生产的client指针|
|string|准备传输的数据|

**返回值**

|返回值类型|解释|
|-|-|
|boolean|成功与否|
|int|底层返回值,调试用|

**例子**

无

---

## esphttp.perform(client, async)

发起http请求

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过esphttp.init生产的client指针|
|boolean|是否异步, 默认情况下是同步, 异步可以等ESPHTTP_EVT事件|

**返回值**

|返回值类型|解释|
|-|-|
|boolean|成功与否|
|int|底层返回值,调试用|

**例子**

无

---

## esphttp.cleanup(client)

清理http客户端数据

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过esphttp.init生产的client指针|

**返回值**

|返回值类型|解释|
|-|-|
|boolean|成功与否|
|int|底层返回值,调试用|

**例子**

无

---

## esphttp.status_code(client)

获取http响应code

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过esphttp.init生产的client指针|

**返回值**

|返回值类型|解释|
|-|-|
|int|例如200/404|

**例子**

无

---

## esphttp.content_length(client)

获取http响应body的长度,不一定大于0

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过esphttp.init生产的client指针|

**返回值**

|返回值类型|解释|
|-|-|
|int|例如123|

**例子**

无

---

## esphttp.read_response(client, len)

读取http响应body

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过esphttp.init生产的client指针|
|int|读取的大小|

**返回值**

|返回值类型|解释|
|-|-|
|string|响应的全部或者片段, 若读取完成,就返回空字符串|

**例子**

无

---

## esphttp.set_header(client, key, value)

设置http请求的header

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过esphttp.init生产的client指针|
|string|header的key, 必须是字符串|
|string|header的value, 必须是字符串|

**返回值**

|返回值类型|解释|
|-|-|
|boolean|成功返回true,否则返回false|
|int|底层返回的状态码,调试用|

**例子**

无

---

## esphttp.get_header(client, key)

获取http响应的header

**参数**

|传入值类型|解释|
|-|-|
|userdata|通过esphttp.init生产的client指针|
|string|header的key, 必须是字符串|

**返回值**

|返回值类型|解释|
|-|-|
|string|成功返回字符串,否则返回nil|

**例子**

无

---

