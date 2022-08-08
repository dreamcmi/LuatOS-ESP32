/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  socket
@summary socket操作库
@version 1.0
@date    2022.2.15
*/
#include "luat_base.h"
#include "luat_msgbus.h"
#include "luat_malloc.h"
#include "stdio.h"
#include <string.h>
#include <sys/param.h>
#include "esp_system.h"
#include "esp_event.h"
// #include "esp_log.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LUAT_LOG_TAG "socket"
#include "luat_log.h"

#define META_SOCKET "SOCKET*"

#define to_socket(L)	((int*)luaL_checkudata(L, 1, META_SOCKET))

/*
创建socket
@api socket.create(sockType)
@int socket.TCP socket.UDP
@return int sock_handle 用于后续操作
@usage
sock = socket.create(socket.TCP)
*/
static int l_socket_create(lua_State *L)
{
    int sockType = luaL_checkinteger(L, 1);
    int sock = socket(AF_INET, sockType, IPPROTO_IP);
    lua_pushinteger(L, sock);
    return 1;
}

typedef struct luat_socket_connect
{
    long cwait_id;
    int socket_id;
    int port;
    char host[32];
}luat_socket_connect_t;

static int l_socket_connect_cb(lua_State *L, void* ptr) {
    rtos_msg_t* msg = (rtos_msg_t*)lua_topointer(L, -1);
    luat_socket_connect_t * tmp = (luat_socket_connect_t *)ptr;
    lua_pushinteger(L, msg->arg1);
    luat_cbcwait(L, tmp->cwait_id, 1);
    return 0;
}

static void t_socket_connect(void* ptr) {
    luat_socket_connect_t * tmp = (luat_socket_connect_t *)ptr;
    struct sockaddr_in dest_addr;

    dest_addr.sin_addr.s_addr = inet_addr(tmp->host);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(tmp->port);

    LLOGD("connect %d host %s port %d", tmp->socket_id, tmp->host, tmp->port);

    int err = connect(tmp->socket_id, &dest_addr, sizeof(struct sockaddr_in6));
    fcntl(tmp->socket_id, F_SETFL, O_NONBLOCK);
    LLOGD("socket connect ret %d", err);
    
    rtos_msg_t msg = {0};
    msg.handler = l_socket_connect_cb;
    msg.ptr = ptr;
    msg.arg1 = err;
    luat_msgbus_put(&msg, 0);

    vTaskDelete(NULL);
}

/*
连接socket
@api socket.connect(sock_handle,ip,port, async)
@int socket 句柄
@string     服务器ip
@int        服务器端口
@boolean    是否异步
@return int err 是否成功
@usage
local cwait = socket.connect(sock, "112.125.89.8", 33863, true)
local ret = cwait:wait()
log.info("socket","connect", err)
*/
static int l_socket_connect(lua_State *L)
{
    struct sockaddr_in dest_addr;
    size_t len = 0;
    int sock = luaL_checkinteger(L, 1);
    const char *host_ip = luaL_checklstring(L, 2, &len);
    int host_port = luaL_checkinteger(L, 3);
    int async = lua_toboolean(L, 4);

    if (async) {
        luat_socket_connect_t * tmp = luat_heap_malloc(sizeof(luat_socket_connect_t));
        memcpy(tmp->host, host_ip, len + 1);
        tmp->port = host_port;
        tmp->cwait_id = luat_pushcwait(L);
        tmp->socket_id = sock;
        int ret = xTaskCreate(t_socket_connect, "scon", 4096, (void *)tmp, 3, NULL);
        LLOGD("socket connect cwait start ret %d", ret);
        if (ret == pdPASS)
            return 1;
        return 0;
    }
    else {
        dest_addr.sin_addr.s_addr = inet_addr(host_ip);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(host_port);
        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
        fcntl(sock, F_SETFL, O_NONBLOCK);
        lua_pushinteger(L, err);
    }
    return 1;
}

/*
发送数据
@api socket.send(sock_handle,data)
@int socket句柄
@string 待发送的数据,只能是字符串
@return int 返回值
@usage
socket.send(sock, "hello lua esp32")
*/
static int l_socket_send(lua_State *L)
{
    size_t len = 0;
    int sock = luaL_checkinteger(L, 1);
    const char *payload = luaL_checklstring(L, 2, &len);
    int err = send(sock, payload, len, 0);
    lua_pushinteger(L, err);
    return 1;
}

/*
接收数据
@api socket.recv(sock_handle)
@int socket句柄
@return string 数据, 可能是nil , 空字符串, 有长度字符串, 3种情况
@return int 若>=0,数据长度, 若<0, 则代表异常,此时数据必为nil,不要再读数据了
@usage
local data, len = socket.recv(sock)
if data then
    log.info("socket", "data", #data, data)
else
    if len < 0 then
        log.info("socket", "closed")
        socket.close(sock)
    end
end
*/
static int app_tcp_recv_timeout_posix(int s, void *mem, size_t len, int flags, int timeout) {
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd = s;

    FD_ZERO( &read_fds );
    FD_SET( fd, &read_fds );

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;

    ret = select( fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv );

    /* Zero fds ready means we timed out */
    if( ret == 0 )
        return 0;

    if( ret < 0 )
    {
        return ret;
    }

    /* This call will not block */
    return recv( fd, mem, len, flags);
}

static int l_socket_recv(lua_State *L)
{
    int sock = luaL_checkinteger(L, 1);
    char rx_buffer[1024];
    int ret = app_tcp_recv_timeout_posix(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, 1);
    if (ret < 0)
    {
        lua_pushnil(L);
        lua_pushinteger(L, ret);
    }
    else
    {
        lua_pushlstring(L, (const char *)rx_buffer, ret);
        lua_pushinteger(L, ret);
    }
    return 2;
}

/*
销毁socket
@api socket.close(sock_handle)
@int sock_handle
@return none
@usage
socket.close(sock)
*/
static int l_socket_close(lua_State *L)
{
    int sock = luaL_checkinteger(L, 1);
    shutdown(sock, 0);
    close(sock);
    return 0;
}

/*
解析域名
@api socket.dns(addr,port,sockType)
@string 域名
@string 端口,默认80
@int socket类型,默认tcp
@return ip
@usage
socket.dns("wiki.luatos.com")
*/
static int l_socket_dns(lua_State *L)
{
    const char *gaddr = luaL_checkstring(L, 1);
    const char *gport = luaL_optstring(L, 2, "80");

    char buf[100];
    struct sockaddr_in *ipv4 = NULL;
    struct addrinfo *result;

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = luaL_optinteger(L, 3, SOCK_STREAM),
    };

    getaddrinfo(gaddr, gport, &hints, &result);
    ipv4 = (struct sockaddr_in *)result->ai_addr;
    inet_ntop(result->ai_family, &ipv4->sin_addr, buf, sizeof(buf));

    lua_pushlstring(L, buf, strlen(buf));
    return 1;
}

/*
绑定IP端口
@api socket.bind(sock_handle,ip,port)
@int sock_handle
@string ip 0.0.0.0表示INADDR_ANY
@int port 端口
@return err
@usage
socket.bind(sock, "0.0.0.0", 8684)
*/
static int l_socket_bind(lua_State *L)
{
    size_t len = 0;
    int sock = luaL_checkinteger(L, 1);
    const char *host_ip = luaL_checklstring(L, 2, &len);
    int port = luaL_checkinteger(L, 3);
    struct sockaddr_storage dest_addr;

    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = inet_addr(host_ip);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(port);
    int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    lua_pushinteger(L, err);
    return 1;
}

/*
socket监听
@api socket.listen(sock,num)
@int sock_handle
@int num 最大数量,默认1
@return err
@usage
socket.listen(sock)
*/
static int l_socket_listen(lua_State *L)
{
    int sock = luaL_checkinteger(L, 1);
    int num = luaL_optinteger(L, 2, 1);

    int err = listen(sock, num);
    lua_pushinteger(L, err);
    return 1;
}

//----------------------------------------------------
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "luat_malloc.h"
#include "luat_msgbus.h"


static int l_socket_accept_cb(lua_State *L, void* ptr) {
    rtos_msg_t* msg = (rtos_msg_t*)lua_topointer(L, -1);
    lua_getglobal(L, "sys_pub");
    if (lua_isnil(L, -1)) {
        return 0;
    }
    lua_pushstring(L, "S_ACCEPT");
    int* tmp = lua_newuserdata(L, sizeof(int));
    *tmp = msg->arg1;
    luaL_setmetatable(L, META_SOCKET);
    lua_pushinteger(L, msg->arg2);
    lua_call(L, 3, 0);
    return 0;
}


static void socket_accept_entry(void* params) {
    int sock = (int)params;
    rtos_msg_t msg = {0};
    int csock = 0;
    struct sockaddr_storage source_addr = {0};
    socklen_t addr_len = sizeof(source_addr);

    msg.handler = l_socket_accept_cb;

    while (1) {
        LLOGI("socket wait accept %d", sock);
        int csock = accept(sock, (struct sockaddr *)&source_addr, &addr_len);
        LLOGI("socket accept %d", csock);
        int flags = fcntl(csock, F_GETFL, 0);
        fcntl(csock, F_SETFL, flags | O_NONBLOCK);
        msg.arg1 = csock;
        msg.arg2 = sock;
        luat_msgbus_put(&msg, 1);
        if (csock <= 1) {
            break;
        }
    }
    LLOGI("end of accept %d", csock);
    vTaskDelete(NULL);
}

/*
socket监听
@api socket.accept(sock, tp)
@int socket套接字
@int 监听类型, 0 同步阻塞(不推荐), 1 异步监听(topic模式), 当前模式0.
@return sock_hanle 同步模式返回socket id, 异步模式返回启动线程的结果true或者nil
@usage
local c = socket.accept(sock)
*/
static int l_socket_accept(lua_State *L)
{
    int sock = luaL_checkinteger(L, 1);
    if (lua_isinteger(L, 2)) {
        LLOGD("start async socket accept for %d", socket);
        int ret = xTaskCreate(socket_accept_entry, "saccept", 4096, (void*)sock, 10, NULL);
        if (ret == pdPASS) {
            lua_pushboolean(L, 1);
            return 1;
        }
        else {
            LLOGW("fail to start accept thread ret %d", ret);
            return 0;
        }
    }
    LLOGD("start sync socket accept for %d", socket);

    struct sockaddr_storage source_addr = {0};
    socklen_t addr_len = sizeof(source_addr);

    int csock = accept(sock, (struct sockaddr *)&source_addr, &addr_len);
    lua_pushinteger(L, csock);
    return 1;
}

static int socket_gc(lua_State *L){
    int* s = to_socket(L);
    if (s == NULL)
        return 0;
    int fd = *s;
    close(fd);
    return 0;
}

static int socket_fd(lua_State *L){
    int* s = to_socket(L);
    if (s == NULL)
        return 0;
    int fd = *s;
    lua_pushinteger(L, fd);
    return 1;
}

static int socket_meta_index(lua_State *L) {
    // void* fdata = lua_touserdata(L, 1);
    // if (fdata == NULL)
    //     return 0;
    // int fd = (int)fdata;
    if (lua_isstring(L, 2)) {
        const char* keyname = luaL_checkstring(L, 2);
        if (!strcmp("__gc", keyname)) {
            lua_pushcfunction(L, socket_gc);
            return 1;
        }
        if (!strcmp("fd", keyname)) {
            lua_pushcfunction(L, socket_fd);
            return 1;
        }
    }
    return 0;
}

#include "rotable2.h"
static const rotable_Reg_t reg_socket[] =
{
    {"create", ROREG_FUNC(l_socket_create)},
    {"connect", ROREG_FUNC(l_socket_connect)},
    {"send", ROREG_FUNC(l_socket_send)},
    {"recv", ROREG_FUNC(l_socket_recv)},
    {"close", ROREG_FUNC(l_socket_close)},
    {"dns", ROREG_FUNC(l_socket_dns)},
    {"bind", ROREG_FUNC(l_socket_bind)},
    {"listen", ROREG_FUNC(l_socket_listen)},
    {"accept", ROREG_FUNC(l_socket_accept)},

    {"TCP", ROREG_INT(SOCK_STREAM)},
    {"UDP", ROREG_INT(SOCK_DGRAM)},
    {"RAW", ROREG_INT(SOCK_RAW)},
    {NULL, ROREG_INT(0)}
};

LUAMOD_API int luaopen_socket(lua_State *L)
{
    luaL_newmetatable(L, META_SOCKET);
    lua_pushcfunction(L, socket_meta_index);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);  /* pop new metatable */

    luat_newlib2(L, reg_socket);

    return 1;
}
