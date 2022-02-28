PROJECT = "sdmmc"
VERSION = "1.0.0"

local sys = require "sys"

sys.taskInit(function()
    sys.wait(1000)
    --使用sdspi模式挂载sd卡
    --c3使用spi2的引脚，cs引脚自定义
    --MISO 10，MOSI 3，SCLK 2，CS，6

    --s3使用SPI2，cs引脚自定义
    --MISO 11，MOSI 12，SCLK 13，CS，4
    --SDIO 引脚自行配置
    --SDIO 初始化
--     ret = sdmmc.init(1,sdmmc.SDMMC_FREQ_DEFAULT,1,13,12,11)
    --SDSPI 初始化
    ret = sdmmc.init(2,sdmmc.SDMMC_FREQ_DEFAULT,4)
    if ret then
        sys.wait(1000)
        --写入sd卡
        if io.writeFile("/sd/test.txt","bb") then
            log.info("sdmmc","写入成功")
        else
            log.error("sdmmc","写入失败")
        end
        sys.wait(1000)
        --读取写入的文件
        log.debug("sdmmc","读取的内容: "..io.readFile("/sd/test.txt"))
        --卸载sd卡
        sys.wait(1000)
        if sdmmc.deinit() then
            log.info("sdmmc","卸载成功")
        else
            log.error("sdmmc","卸载失败")
        end
    else
        log.error("sdmmc","挂载失败，请检查SD卡是否正常或文件系统是否为fat32")
    end
    while 1 do
        sys.wait(1000)
    end
end)


-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
