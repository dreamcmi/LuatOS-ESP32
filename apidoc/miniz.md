# miniz - esp32专用miniz压缩库

## miniz.compress(str)

miniz压缩

**参数**

| 传入值类型 | 解释         |
| ---------- | ------------ |
| string     | 待压缩字符串 |

**返回值**

| 返回值类型 | 解释         |
| ---------- | ------------ |
| string     | 压缩结果     |
| int        | 压缩结果长度 |

**例子**

```lua
local s_str = "Hello world! Hello world! Hello world! Hello world! Hello world! Hello world! "
data, len = miniz.compress(s_str)
```

---

## miniz.compress(str)

miniz解压缩

**参数**

| 传入值类型 | 解释         |
| ---------- | ------------ |
| string     | 待压缩字符串 |

**返回值**

| 返回值类型 | 解释         |
| ---------- | ------------ |
| string     | 解压结果     |
| int        | 解压结果长度 |

**例子**

```lua
udata, ulen = miniz.uncompress(data)
```

---

