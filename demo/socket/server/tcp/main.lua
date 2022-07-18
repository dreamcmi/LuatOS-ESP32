PROJECT = "socket-server"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"

-- 客户端数据处理函数
function handle_client(csock)
    local s = csock:fd()
    while 1 do
        -- 尝试接收数据
        local data, len = socket.recv(s)
        log.info("socket", "client", s, data, data and #data or 0)
        if data ~= nil then
            -- 若数据存在, 处理之, 下列逻辑为测试逻辑
            if data == "close" then
                log.info("socket", "close")
                break
            end
        elseif len and len < 0 then
            log.info("socket", "client closed")
            break
        end
        sys.wait(50)
    end
    socket.close(s)
    log.info("socket", "client", "处理结束")
end

sys.taskInit(
    function()
        log.info("wlan", "wlan_init:", wlan.init())
        wlan.setMode(wlan.STATION)
        wlan.connect("uiot", "1234567890", 1)
        -- 等到成功获取ip就代表连上局域网了
        result, data = sys.waitUntil("IP_READY", 30000)
        log.info("wlan", "IP_READY", result, data)

        while 1 do
            
            log.info("socket", "begin socket")
            local sock = socket.create(socket.TCP) -- tcp

            log.info("socket.bind", socket.bind(sock, "0.0.0.0", 8684))
            log.info("socket.listen", socket.listen(sock))

            if socket.accept(sock, 1) then
                while 1 do
                    local ret, csock, tmp = sys.waitUntil("S_ACCEPT", 30000)
                    log.info("socket", "accept", ret, csock, tmp)
                    if ret and csock then
                        -- 启动异步处理函数去读取
                        sys.taskInit(handle_client, csock)
                    end
                end
            else
                log.warn("socket", "监听失败, 5秒后重试")
            end
            socket.close(sock)
        end
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
