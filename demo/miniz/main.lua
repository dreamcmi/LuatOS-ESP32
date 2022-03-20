PROJECT = "miniz"
VERSION = "1.0.0"

local sys = require "sys"
local s_str = "Hello world! Hello world! Hello world! Hello world! Hello world! Hello world! "

sys.taskInit(
    function()
        log.info("SRC_STR",s_str)
        log.info("SRC_LEN",s_str:len())
        
        cdata, clen = miniz.compress(s_str)
        log.info("miniz.compress", "len is:", clen, "data is:", cdata)

        udata, ulen = miniz.uncompress(cdata)
        log.info("miniz.uncompress", "len is:", ulen, "data is:", udata)
    end
)

sys.run()
