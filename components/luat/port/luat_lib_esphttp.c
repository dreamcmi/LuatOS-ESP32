#include "luat_base.h"
#include "luat_msgbus.h"
#include "luat_malloc.h"

/*
@module  esphttp
@summary http客户端
@version 1.0
@date    2021.12.22
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
// #include "esp_tls.h"
// #include "esp_crt_bundle.h"
#include "esp_http_client.h"

#define LUAT_LOG_TAG "esphttp"
#include "luat_log.h"

static esp_err_t l_esphttp_event_handler(esp_http_client_event_t *evt) {
    rtos_msg_t msg = {0};
    msg.handler = NULL;
    // msg.
    return ESP_OK;
}

/*
初始化一个URL请求
@api esphttp.init(method, url)
@string 请求的方法, 例如 GET/POST/PUT
@string 网址
@return userdata 如果成功就返回client指针,否则返回nil
@usage

local httpc = esphttp.init("GET", "http://luatos.com/123.txt")
if httpc then
    local ok, err = esphttp.perform(httpc)
    if ok then
        local code = esphttp.state_code(httpc)
        log.info("esphttp", "code", code, "len", esphttp.content_length())
        if code == 200 then
            log.info("esphttp", "data", esphttp.read_response(httpc, 1024))
        end
    else
        log.warn("esphttp", "bad perform", err)
    end
    esphttp.cleanup(httpc)
end
*/
static int l_esphttp_init(lua_State *L) {
    esp_http_client_config_t conf = {0};
    if (!lua_isinteger(L, 1) || !lua_isstring(L, 2)) {
        LLOGW("init with method and url!!!");
    }
    conf.method = luaL_checkinteger(L, 1);
    conf.url = luaL_checkstring(L, 2);
    //conf.event_handler = l_esphttp_event_handler; // TODO 仅https支持异步,这就很尴尬了
    esp_http_client_handle_t client = esp_http_client_init(&conf);
    if (client != NULL) {
        lua_pushlightuserdata(L, client);
        return 1;
    }
    return 0;
}

/*
设置请求的body
@api esphttp.post_field(client, data)
@userdata 通过esphttp.init生产的client指针
@string 准备传输的数据
@return boolean 成功与否
@return int 底层返回值,调试用
*/
static int l_esphttp_set_post_field(lua_State *L) {
    if (!lua_islightuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esp_http_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);
    if (len > 0) {  // TODO 这里的buff是malloc的, 是否需要自行释放呢? API没有说
        char *buff = (char *)malloc(len);
        memcpy(buff, data, len);
        data = (const char* )buff;
    }
    esp_err_t err = esp_http_client_set_post_field(client, data, len);
    lua_pushboolean(L, err == 0 ? 1 : 0);
    lua_pushinteger(L, err);
    return 2;
}

/*
发起http请求
@api esphttp.perform(client)
@userdata 通过esphttp.init生产的client指针
@return boolean 成功与否
@return int 底层返回值,调试用
*/
static int l_esphttp_perform(lua_State *L) {
    if (!lua_islightuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esp_http_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esp_err_t err = esp_http_client_perform(client);
    lua_pushboolean(L, err == 0 ? 1 : 0);
    lua_pushinteger(L, err);
    return 2;
}

/*
清理http客户端数据
@api esphttp.cleanup(client)
@userdata 通过esphttp.init生产的client指针
@return boolean 成功与否
@return int 底层返回值,调试用
*/
static int l_esphttp_cleanup(lua_State *L) {
    if (!lua_islightuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esp_http_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esp_http_client_cleanup(client);
    return 0;
}

/*
获取http响应code
@api esphttp.status_code(client)
@userdata 通过esphttp.init生产的client指针
@return int 例如200/404
*/
static int l_esphttp_get_status_code(lua_State *L) {
    if (!lua_islightuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esp_http_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    int code = esp_http_client_get_status_code(client);
    lua_pushinteger(L, code);
    return 1;
}

/*
获取http响应body的长度,不一定大于0
@api esphttp.content_length(client)
@userdata 通过esphttp.init生产的client指针
@return int 例如123
*/
static int l_esphttp_get_content_length(lua_State *L) {
    if (!lua_islightuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esp_http_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    int code = esp_http_client_get_content_length(client);
    lua_pushinteger(L, code);
    return 1;
}

/*
读取http响应body
@api esphttp.read_response(client, len)
@userdata 通过esphttp.init生产的client指针
@int 读取的大小
@return string 响应的全部或者片段, 若读取完成,就返回空字符串
*/
static int l_esp_http_client_read_response(lua_State *L) {
    if (!lua_islightuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esp_http_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    int len = luaL_checkinteger(L, 2);
    luaL_Buffer buff;
    luaL_buffinitsize(L, &buff, len);
    len = esp_http_client_read_response(client, buff.b, len);
    luaL_pushresultsize(&buff, len);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_esphttp[] =
{
	{ "init",          l_esphttp_init ,          0},
    { "post_field", l_esphttp_set_post_field, 0},
    { "perform", l_esphttp_perform, 0},
    { "status_code", l_esphttp_get_status_code, 0},
    { "content_length", l_esphttp_get_content_length, 0},
    { "read_response", l_esp_http_client_read_response, 0},
    { "cleanup", l_esphttp_cleanup, 0},

    // METHODS
    { "GET", NULL, HTTP_METHOD_GET},
    { "POST", NULL, HTTP_METHOD_POST},
    { "PUT", NULL, HTTP_METHOD_PUT},
    { "PATCH", NULL, HTTP_METHOD_PATCH},
    { "DELETE", NULL, HTTP_METHOD_DELETE},

	{ NULL,          NULL ,          0}
};

LUAMOD_API int luaopen_esphttp( lua_State *L ) {
    luat_newlib(L, reg_esphttp);
    return 1;
}

