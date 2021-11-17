#include "luat_base.h"
#include "stdio.h"
#include <string.h>
#include <sys/param.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

static const char *TAG = "lsocket";

/*
创建socket
@api socket.creat(sockType)
@int socket.TCP socket.UDP
@return int sock_handle 用于后续操作
@usage
sock = socket.creat(socket.TCP)
*/
static int l_socket_creat(lua_State *L)
{
    int sockType = luaL_checkinteger(L, 1);
    int sock = socket(AF_INET, sockType, IPPROTO_IP);
    lua_pushinteger(L, sock);
    return 1;
}

/*
连接socket
@api socket.connect(sock_handle,ip,port)
@int sock_handle
@string ip
@int port
@return int err
@usage
err = socket.connect(sock, "112.125.89.8", 33863)
log.info("socket","connect",err)
*/
static int l_socket_connect(lua_State *L)
{
    struct sockaddr_in dest_addr;
    size_t len = 0;
    int sock = luaL_checkinteger(L, 1);
    const char *host_ip = luaL_checklstring(L, 2, &len);
    int host_port = luaL_checkinteger(L, 3);

    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(host_port);

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
    lua_pushinteger(L, err);
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
接收数据(非阻塞)
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
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
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

#include "rotable.h"
static const rotable_Reg reg_socket[] =
    {
        {"creat", l_socket_creat, 0},
        {"connect", l_socket_connect, 0},
        {"send", l_socket_send, 0},
        {"recv", l_socket_recv, 0},
        {"close", l_socket_close, 0},

        {"TCP", NULL, SOCK_STREAM},
        {"UDP", NULL, SOCK_DGRAM},
        {"RAW", NULL, SOCK_RAW},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_socket(lua_State *L)
{
    luat_newlib(L, reg_socket);
    return 1;
}
