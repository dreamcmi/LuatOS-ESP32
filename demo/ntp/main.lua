PROJECT = "ntpp"
VERSION = "1.0.0"

local sys = require "sys"

sys.taskInit(
    function()
        log.info("wlan", "wlan_init:", wlan.init())
        log.info("mode", wlan.setMode(wlan.STATION))
        log.info("connect", wlan.connect("xxxx", "123456789"))
        -- 等待连上路由,此时还没获取到ip
        result, _ = sys.waitUntil("WLAN_STA_CONNECTED")
        log.info("wlan", "WLAN_STA_CONNECTED", result)
        -- 等到成功获取ip就代表连上局域网了
        result, data = sys.waitUntil("IP_READY")
        log.info("wlan", "IP_READY", result, data)
        -- 北京时间,具体参数请看(https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html)
        ntp.settz("CST-8") 
        ntp.init("ntp.ntsc.ac.cn")
    end
)

sys.subscribe(
    "NTP_SYNC_DONE",
    function()
        log.info("ntp", "done")
        log.info("date", os.date())
    end
)

sys.run()
