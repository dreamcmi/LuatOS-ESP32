# PATCH

此文件夹针对ESP-IDF进行修补

## How To Use？

patch文件命名规则：`序号_IDF版本号_修复原因.patch`

复制patch文件到idf根目录，然后执行`git apply xxx.patch `

注意：因为Win10的PowerShell默认是UTF16编码，这会导致修补失败,所以建议使用cmd。