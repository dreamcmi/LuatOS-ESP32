PROJECT = "ntpp"
VERSION = "1.0.0"

local sys = require "sys"

sys.taskInit(
    function()
        log.info("wlan", "wlan_init:", wlan.init())
        log.info("mode", wlan.setMode(wlan.STATION))
        log.info("connect", wlan.connect("xxxx", "123456789"))

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
