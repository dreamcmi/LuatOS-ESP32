--- 模块功能：lcddemo
-- @module lcd
-- @author Dozingfiretruck
-- @release 2021.01.25
-- LuaTools需要PROJECT和VERSION这两个信息
PROJECT = "lcddemo"
VERSION = "1.0.0"

log.info("main", PROJECT, VERSION)

-- sys库是标配
_G.sys = require("sys")

-- 添加硬狗防止程序卡死
-- wdt.init(15000)--初始化watchdog设置为15s
-- sys.timerLoopStart(wdt.feed, 10000)--10s喂一次狗

spi_lcd = spi.deviceSetup(2, 7, 0, 0, 8, 40000000, spi.MSB, 1, 1)

-- lcd.init("st7789", {
--     port = "device",
--     pin_dc = 6,
--     pin_rst = 10,
--     direction = 0,
--     w = 240,
--     h = 320,
--     xoffset = 0,
--     yoffset = 0
-- }, spi_lcd)
-- log.info("lcd.init",
-- lcd.init("st7735v",{port = "device",pin_dc = 6, pin_rst = 10,direction = 0,w = 80,h = 160,xoffset = 26,yoffset = 1},spi_lcd)
-- 屏幕反色是因为驱动里有反色的命令，用户自行注释luatos主库-->lcd-->luat_lcd_st7735v.c文件中的 lcd_write_cmd(conf,0x21)然后自行编译源码

--下面为custom方式示例,自己传入lcd指令来实现驱动,示例以st7735v做展示
log.info("lcd.init",
lcd.init("custom",{
    port = "device",
    pin_dc = 6, 
    pin_rst = 10,
    w = 160,
    h = 80,
    xoffset = 0,
    yoffset = 24,
    sleepcmd = 0x10,
    wakecmd = 0x11,
    initcmd = {--0001 delay  0002 cmd  0003 data
        0x00020011,0x00010078,--0x00020021, -- 反显
        0x000200B1,0x00030002,0x00030035,
        0x00030036,0x000200B2,0x00030002,
        0x00030035,0x00030036,0x000200B3,
        0x00030002,0x00030035,0x00030036,
        0x00030002,0x00030035,0x00030036,
        0x000200B4,0x00030007,0x000200C0,
        0x000300A2,0x00030002,0x00030084,
        0x000200C1,0x000300C5,0x000200C2,
        0x0003000A,0x00030000,0x000200C3,
        0x0003008A,0x0003002A,0x000200C4,
        0x0003008A,0x000300EE,0x000200C5,
        0x0003000E,0x00020036,0x00030078,
        0x000200E0,0x00030012,0x0003001C,
        0x00030010,0x00030018,0x00030033,
        0x0003002C,0x00030025,0x00030028,
        0x00030028,0x00030027,0x0003002F,
        0x0003003C,0x00030000,0x00030003,
        0x00030003,0x00030010,0x000200E1,
        0x00030012,0x0003001C,0x00030010,
        0x00030018,0x0003002D,0x00030028,
        0x00030023,0x00030028,0x00030028,
        0x00030026,0x0003002F,0x0003003B,
        0x00030000,0x00030003,0x00030003,
        0x00030010,0x0002003A,0x00030005,
        0x00020029,
    },
    },
    spi_lcd))


log.info("lcd.drawLine", lcd.drawLine(20, 20, 150, 20, 0x001F))
log.info("lcd.drawRectangle", lcd.drawRectangle(20, 40, 120, 70, 0xF800))
log.info("lcd.drawCircle", lcd.drawCircle(50, 50, 20, 0x0CE0))

sys.taskInit(function() while 1 do sys.wait(500) end end)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
