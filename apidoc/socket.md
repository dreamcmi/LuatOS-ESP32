# socket - socket操作库

## socket.create(sockType)

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
sock = socket.create(socket.TCP)

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

| 返回值类型 | 解释 |
| ---------- | ---- |
| string     | ip   |

**例子**

```lua
local ip = socket.dns("wiki.luatos.com")
```

---

## socket.bind(sock_handle,ip,port)

绑定IP端口

**参数**

| 传入值类型 | 解释                      |
| ---------- | ------------------------- |
| int        | sock_handle               |
| string     | ip  0.0.0.0表示INADDR_ANY |
| int        | port 端口                 |

**返回值**

| 返回值类型 | 解释 |
| ---------- | ---- |
| int        | err  |

**例子**

```lua
socket.bind(sock, "0.0.0.0", 8684)
```

---

## socket.listen(sock,num)

socket监听

**参数**

| 传入值类型 | 解释               |
| ---------- | ------------------ |
| int        | sock_handle        |
| int        | 最大监听数量,默认1 |

**返回值**

| 返回值类型 | 解释 |
| ---------- | ---- |
| int        | err  |

**例子**

```lua
socket.listen(sock)
```

---

## socket.accept(sock)

socket通过

**参数**

| 传入值类型 | 解释        |
| ---------- | ----------- |
| int        | sock_handle |

**返回值**

| 返回值类型 | 解释                      |
| ---------- | ------------------------- |
| int        | 链接设备对应的sock_handle |

**例子**

```lua
local c = socket.accept(sock)
```

---



