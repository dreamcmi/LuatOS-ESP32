# adc.open

```lua
adc.open(id)
```

打开adc通道

## 参数表

Name | Type | Description
-----|------|--------------
`id`|`int`| 通道id,与具体设备有关,通常从0开始

> id可选：32-39

## 返回值

> `boolean`: 打开结果

## 调用示例

```lua
-- 打开adc通道2,并读取
if adc.open(32) then
    log.info("adc", adc.read(32))
end
adc.close(32)
```


--------------------------------------------------
# adc.read

```lua
adc.read(id)
```

读取adc通道

## 参数表

Name | Type | Description
-----|------|--------------
`id`|`int`| 通道id,与具体设备有关,通常从0开始

> id可选：32-39

## 返回值

No. | Type | Description
----|------|--------------
1 |`int`| 原始值
2 |`int`| 计算后的值

## 调用示例

```lua
-- 打开adc通道2,并读取
if adc.open(32) then
    log.info("adc", adc.read(32))
end
adc.close(32)
```


--------------------------------------------------
# adc.close

```lua
adc.close(id)
```

关闭adc通道

## 参数表

Name | Type | Description
-----|------|--------------
`id`|`int`| 通道id,与具体设备有关,通常从0开始

> id可选：32-39

## 返回值

> *无返回值*

## 调用示例

```lua
-- 打开adc通道2,并读取
if adc.open(32) then
    log.info("adc", adc.read(32))
end
adc.close(32)
```

