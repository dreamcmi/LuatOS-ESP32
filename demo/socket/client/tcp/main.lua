PROJECT = "socket-tcp-client"
VERSION = "1.0.0"

-- 一定要添加sys.lua !!!!
local sys = require "sys"
require "sysplus"

--[[
本demo需要 2022-08-08 之后编译的固件
]]

-- 联网函数, 不成功不返回
function wlan_connect()
    local ret = wlan.init()
    log.info("wlan", "wlan_init:", ret)
    wlan.setMode(wlan.STATION)
    -- 务必修改为实际wifi信息, 硬件限制只能连2.4G的wifi
    wlan.connect("uiot", "1234567890", 1) -- 第三个参数代表自动重连

    -- 等到成功获取ip就代表连上局域网了
    while 1 do
        result, data = sys.waitUntil("IP_READY", 30000)
        log.info("wlan", "IP_READY", result, data)
        -- 若result == true, 代表wifi连接成功了
        -- data 是 本地ip, 可按自身业务存储
        if result then
            -- 把自身IP存一下
            --self_ip = data 
            break
        end
    end
end

sys.taskInit(
    function()
        -- 联网
        wlan_connect()
        sys.wait(100)
        log.info("socket", "begin socket")
        local ret = false
        while 1 do
            local sock = socket.create(socket.TCP) -- tcp
            -- 连接到 netlab.luatos.com
            -- ip和端口要自行登录 netlab.luatos.com 获取
            repeat
                -- 根据需要连接的服务器写, 第4个参数启用非阻塞
                -- 注意不能填127.0.0.1, 如果是连接电脑的端口, 填电脑的局域网ip
                cw = socket.connect(sock, "112.125.89.8", 36153, true)
                if cw then
                    -- 非阻塞式等待
                    ret = cw:wait()
                    log.info("socket", "cwait", "connect", ret)
                    if ret and ret == 0 then
                        -- 连上了, 退出循环
                        break
                    end
                else
                    -- 没连上, 关掉已有的socket, 
                    log.info("socket", "start socket.connect fail")
                    socket.close(sock)
                    -- 重新建socket
                    sys.wait(100)
                    sock = socket.create(socket.TCP)
                end
                sys.wait(3000)  -- 重试间隔
            until (true)

            len = socket.send(sock, "hello lua esp32c3")
            log.info("socket", "sendlen", len)

            -- 若发送失败, len就小于0, 下面的逻辑就直接跳过了
            local counter = 0
            while len >= 0 do
                -- recv是非阻塞, 会马上返回
                local data, len = socket.recv(sock)
                -- 若data存在, 且长度大于0, 那就是真的收到数据了
                if data ~= nil and len > 0 then
                    -- 这里是演示服务器下发close字符串, 属于业务展示,而非真的"close"事件
                    if data == "close" then
                        log.info("socket", "server say","close")
                        break -- 退出循环, 在后续代码中关闭close
                    end
                    log.info("socket", "len", len)
                    log.info("socket", "data", data)
                -- 若 len 小于 0, 带代表出错, socket关掉之
                elseif len and len < 0 then
                    log.info("socket", "closed")
                    break
                end
                -- 这里模拟一下心跳
                if len and len == 0 then
                    -- 没数据, 就需要心跳计数
                    counter = counter + 1
                else
                    -- 有数据就不需要心跳了
                    counter = 0
                end
                if counter > 100 then
                    counter = 0
                    len = socket.send(sock, "my heartbeat")
                end
                sys.wait(50) -- 延时是为了让出cpu给其他task, 建议大于10ms
            end
            -- 关闭socket,清理资源
            socket.close(sock)
            collectgarbage("collect")
            collectgarbage("collect")
            log.info("socket", "end")
            sys.wait(2000) -- 2000ms后重连
        end
    end
)

-- 用户代码已结束---------------------------------------------
-- 结尾总是这一句
sys.run()
-- sys.run()之后后面不要加任何语句!!!!!
