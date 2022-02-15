# socket - socket操作库

## socket.creat(sockType)

创建socket

**参数**

|传入值类型|解释|
|-|-|
|int|socket.TCP socket.UDP|

**返回值**

|返回值类型|解释|
|-|-|
|int|sock_handle 用于后续操作|

**例子**

```lua
sock = socket.creat(socket.TCP)

```

---

## socket.connect(sock_handle,ip,port)

连接socket

**参数**

|传入值类型|解释|
|-|-|
|int|sock_handle|
|string|ip|
|int|port|

**返回值**

|返回值类型|解释|
|-|-|
|int|err|

**例子**

```lua
err = socket.connect(sock, "112.125.89.8", 33863)
log.info("socket","connect",err)

```

---

## socket.send(sock_handle,data)

发送数据

**参数**

|传入值类型|解释|
|-|-|
|int|sock_handle|
|string|data|

**返回值**

|返回值类型|解释|
|-|-|
|int|err|

**例子**

```lua
socket.send(sock, "hello lua esp32")

```

---

## socket.recv(sock_handle)

接收数据

**参数**

|传入值类型|解释|
|-|-|
|int|sock_handle|

**返回值**

|返回值类型|解释|
|-|-|
|string|data|
|int|len|

**例子**

```lua
local data, len = socket.recv(sock)

```

---

## socket.close(sock_handle)

销毁socket

**参数**

|传入值类型|解释|
|-|-|
|int|sock_handle|
|return|none|

**返回值**

无

**例子**

```lua
socket.close(sock)

```

---

## socket.dns(addr,port,sockType)

解析域名

**参数**

|传入值类型|解释|
|-|-|
|string|域名|
|string|端口,默认80|
|int|socket类型,默认tcp|
|return|ip|

**返回值**

无

**例子**

```lua
socket.dns("wiki.luatos.com")

```

---

