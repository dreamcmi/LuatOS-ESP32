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
    struct sockaddr_in dst;
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
    int err = connect(tmp->socket_id, &tmp->dst, sizeof(struct sockaddr_in6));
    fcntl(tmp->socket_id, F_SETFL, O_NONBLOCK);
    
    rtos_msg_t msg = {0};
    msg.handler = l_socket_connect_cb;
    msg.ptr = ptr;
    msg.arg1 = err;
    luat_msgbus_put(&msg, 0);
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

    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(host_port);

    if (async) {
        luat_socket_connect_t * tmp = luat_heap_malloc(sizeof(luat_socket_connect_t));
        memcpy(&tmp->dst, &dest_addr, sizeof(struct sockaddr_in));
        tmp->cwait_id = luat_pushcwait(L);
        int ret = xTaskCreate(t_socket_connect, "scon", 4096, (void *)tmp, 3, NULL);
        LLOGD("socket connect cwait start ret %d", ret);
        return 1;
    }
    else {
        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
        fcntl(sock, F_SETFL, O_NONBLOCK);
        lua_pushinteger(L, err);
    }
    return 1;
}

/*
发送数据
@api socket.send(sock_handle,data)
@int sock_handle
@string data
@return int err
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
@int sock_handle
@return string data
@return int len
@usage
local data, len = socket.recv(sock)
*/
static int l_socket_recv(lua_State *L)
{
    int sock = luaL_checkinteger(L, 1);
    char rx_buffer[1024];
    int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    if (len < 0)
    {
        // ESP_LOGE(TAG, "recv failed: errno %d", errno);
        return 0;
    }
    else
    {
        lua_pushlstring(L, (const char *)rx_buffer, len);
        lua_pushinteger(L, len);
        return 2;
    }
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

/*
socket通过
@api socket.accept(sock)
@int sock_handle
@return sock_hanle 连接设备对应的handle
@usage
local c = socket.accept(sock)
*/
static int l_socket_accept(lua_State *L)
{
    int sock = luaL_checkinteger(L, 1);
    struct sockaddr_storage source_addr;
    socklen_t addr_len = sizeof(source_addr);

    int csock = accept(sock, (struct sockaddr *)&source_addr, &addr_len);
    lua_pushinteger(L, csock);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_socket[] =
    {
        {"create", l_socket_create, 0},
        {"connect", l_socket_connect, 0},
        {"send", l_socket_send, 0},
        {"recv", l_socket_recv, 0},
        {"close", l_socket_close, 0},
        {"dns", l_socket_dns, 0},
        {"bind", l_socket_bind, 0},
        {"listen", l_socket_listen, 0},
        {"accept", l_socket_accept, 0},

        {"TCP", NULL, SOCK_STREAM},
        {"UDP", NULL, SOCK_DGRAM},
        {"RAW", NULL, SOCK_RAW},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_socket(lua_State *L)
{
    luat_newlib(L, reg_socket);
    return 1;
}
