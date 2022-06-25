/*
 * SPDX-FileCopyrightText: 2021-2022 Wendal <wendal1985@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

typedef struct resp_data {
    size_t len;
    char buff[4];
}resp_data_t;

typedef struct esphttpc
{
    uint8_t started;
    char* body;
    long cwait_topic_id;
    esp_http_client_handle_t client;
    int lua_ref_id;
}esphttpc_t;

static int l_esphttp_event_cb(lua_State *L, void* ptr) {
    rtos_msg_t* msg = (rtos_msg_t*)lua_topointer(L, -1);
    esphttpc_t* httpc = (esphttpc_t*)msg->ptr;
    //LLOGD("esphttp event %d", msg->arg1);
    if (httpc && httpc->cwait_topic_id) {
        long cwait_id = httpc->cwait_topic_id;
        httpc->cwait_topic_id = 0;
        lua_pushinteger(L, msg->arg1);
        if (msg->arg1 == HTTP_EVENT_ON_DATA) {
            resp_data_t* re = (resp_data_t*)msg->arg2;
            lua_pushlstring(L, re->buff, re->len);
            luat_heap_free(re);
        }
        else {
            lua_pushnil(L);
        }
        luat_cbcwait(L, httpc->cwait_topic_id, 2);
        return 0; 
    }
    //LLOGD("call sys_pub");
    lua_getglobal(L, "sys_pub");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, "ESPHTTP_EVT");
        if (httpc->lua_ref_id == 0) { // 已经释放了? 不可能吧
            return 0;
        }
        lua_geti(L, LUA_REGISTRYINDEX, httpc->lua_ref_id);
        if (msg->arg1 == HTTP_EVENT_ON_FINISH) { // 请求已经完成, 释放httpc的引用
            luaL_unref(L, LUA_REGISTRYINDEX, httpc->lua_ref_id);
            httpc->lua_ref_id = 0;
        }
        lua_pushinteger(L, msg->arg1);
        if (msg->arg1 == HTTP_EVENT_ON_DATA) {
            resp_data_t* re = (resp_data_t*)msg->arg2;
            //LLOGD("resp_data_t %p", re);
            //LLOGD("resp_data_t %p %d", re, re->len);
            lua_pushlstring(L, re->buff, re->len);
            //LLOGD("free resp_data_t %p %d", re, re->len);
            luat_heap_free(re);
            //LLOGD("lua_call sys_pub %p %d", re, re->len);
            lua_call(L, 4, 0);
        }
        else {
            lua_call(L, 3, 0);
        }
    }
    return 0;
}

static esp_err_t l_esphttp_event_handler(esp_http_client_event_t *evt) {
    rtos_msg_t msg = {0};
    msg.handler = l_esphttp_event_cb;
    msg.arg1 = evt->event_id;
    msg.ptr = evt->user_data;
    if (evt->event_id == HTTP_EVENT_ON_DATA)
    {
        resp_data_t* re = luat_heap_malloc(evt->data_len + 4);
        if (re == NULL) {
            LLOGE("out of memory when malloc http resp");
            return ESP_OK;
        }
        re->len = evt->data_len;
        memcpy(re->buff, evt->data, evt->data_len);
        msg.arg2 = (uint32_t) re;
        int ret = luat_msgbus_put(&msg, 0);
        if (ret != 0) {
            LLOGE("luat_msgbus_put ret %d", ret);
            luat_heap_free(re);
            return ESP_OK;
        }
    }
    else { // 缺省, 直接给
        luat_msgbus_put(&msg, 0);
    }
    //LLOGD("event %d", evt->event_id);
    return ESP_OK;
}

/*
初始化一个URL请求
@api esphttp.init(method, url)
@string 请求的方法, 例如 GET/POST/PUT
@string 网址
@return userdata 如果成功就返回client指针,否则返回nil
@usage

-- 异步写法
local httpc = esphttp.init(esphttp.GET, "http://httpbin.org/get")
if httpc then
    local ok, err = esphttp.perform(httpc, true)
    if ok then
        while 1 do
            local result, c, ret, data = sys.waitUntil("ESPHTTP_EVT", 20000)
            if c == httpc then
                if ret == esphttp.EVENT_ERROR or ret == esphttp.EVENT_DISCONNECTED then
                    break
                end
                if ret == esphttp.EVENT_ON_DATA and esphttp.status_code(httpc) == 200 then
                    log.info("data", "resp", data)
                end
            end
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
        return 0;
    }
    esphttpc_t* httpc = lua_newuserdata(L, sizeof(esphttpc_t));
    if (httpc == NULL) {
        LLOGW("out of memory when malloc httpc");
        return 0;
    }
    memset(httpc, 0, sizeof(esphttpc_t));

    conf.method = luaL_checkinteger(L, 1);
    conf.url = luaL_checkstring(L, 2);
    conf.event_handler = l_esphttp_event_handler;
    conf.keep_alive_enable = false;
    conf.user_data = (void*)httpc;
    esp_http_client_handle_t client = esp_http_client_init(&conf);
    if (client != NULL) {
        lua_pushvalue(L, -1);
        httpc->lua_ref_id = luaL_ref(L, LUA_REGISTRYINDEX);
        httpc->client = client;
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
    if (!lua_isuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esphttpc_t* httpc = lua_touserdata(L, 1);
    esp_http_client_handle_t client = httpc->client;
    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);
    if (len > 0) {  // TODO 这里的buff是malloc的, 是否需要自行释放呢? API没有说
        char *buff = (char *)luat_heap_malloc(len);
        if (buff == NULL) {
            LLOGE("out of memory when malloc httpc body");
            return 0;
        }
        memcpy(buff, data, len);
        data = (const char* )buff;
        httpc->body = buff;
    }
    esp_err_t err = esp_http_client_set_post_field(client, data, len);
    lua_pushboolean(L, err == 0 ? 1 : 0);
    lua_pushinteger(L, err);
    return 2;
}

static void esphttp_client_perform_t(void* ptr) {
    esp_http_client_handle_t client = (esp_http_client_handle_t)ptr;
    esp_err_t err = esp_http_client_perform(client);
    LLOGD("esphttp_client_perform async ret %d", err);
    vTaskDelete(NULL);
}

/*
发起http请求
@api esphttp.perform(client, async)
@userdata 通过esphttp.init生产的client指针
@boolean 是否异步, 默认情况下是同步, 异步可以等ESPHTTP_EVT事件
@return boolean 成功与否
@return int 底层返回值,调试用
*/
static int l_esphttp_perform(lua_State *L) {
    if (!lua_isuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esphttpc_t* httpc = lua_touserdata(L, 1);
    esp_http_client_handle_t client = httpc->client;
    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    int ret = xTaskCreate(esphttp_client_perform_t, "esphttp", 8*1024, client, 20, NULL);
    if (pdPASS == ret)
    {
        lua_pushboolean(L, 1);
        lua_pushinteger(L, 0);

    }
    else {
        lua_pushboolean(L, 0);
        lua_pushinteger(L, ret);
    }
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
    if (!lua_isuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    esphttpc_t* httpc = lua_touserdata(L, 1);
    esp_http_client_handle_t client = httpc->client;
    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    if (httpc->body) {
        free(httpc->body);
        httpc->body = NULL;
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
    if (!lua_isuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        lua_pushinteger(L, 0);
        return 1;
    }
    esphttpc_t* httpc = lua_touserdata(L, 1);
    esp_http_client_handle_t client = httpc->client;

    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        lua_pushinteger(L, 0);
        return 1;
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
    if (!lua_isuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        lua_pushinteger(L, 0);
        return 1;
    }
    esphttpc_t* httpc = lua_touserdata(L, 1);
    esp_http_client_handle_t client = httpc->client;
    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        lua_pushinteger(L, 0);
        return 1;
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
    if (!lua_isuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }

    esphttpc_t* httpc = lua_touserdata(L, 1);
    esp_http_client_handle_t client = httpc->client;

    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    int len = luaL_checkinteger(L, 2);
    luaL_Buffer buff;
    luaL_buffinitsize(L, &buff, len);
    //LLOGD("try read %d %p", len, client);
    len = esp_http_client_read(client, buff.b, len);
    //LLOGD("resp read %d %p", len, client);
    luaL_pushresultsize(&buff, len);
    return 1;
}

/*
设置http请求的header
@api esphttp.set_header(client, key, value)
@userdata 通过esphttp.init生产的client指针
@string header的key, 必须是字符串
@string header的value, 必须是字符串
@return boolean 成功返回true,否则返回false
@return int 底层返回的状态码,调试用
*/
static int l_esp_http_client_set_header(lua_State* L) {
    if (!lua_isuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        lua_pushboolean(L, 0);
        return 1;
    }

    esphttpc_t* httpc = lua_touserdata(L, 1);
    esp_http_client_handle_t client = httpc->client;

    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        lua_pushboolean(L, 0);
        return 1;
    }
    const char* key = luaL_checkstring(L, 2);
    const char* value = luaL_checkstring(L, 3);
    esp_err_t err = esp_http_client_set_header(client, key, value);
    lua_pushboolean(L, err == 0 ? 1 : 0);
    lua_pushinteger(L, err);
    return 2;
}

/*
获取http响应的header
@api esphttp.get_header(client, key)
@userdata 通过esphttp.init生产的client指针
@string header的key, 必须是字符串
@return string 成功返回字符串,否则返回nil
*/
static int l_esp_http_client_get_header(lua_State* L) {
    if (!lua_isuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }

    esphttpc_t* httpc = lua_touserdata(L, 1);
    esp_http_client_handle_t client = httpc->client;

    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        return 0;
    }
    const char* key = luaL_checkstring(L, 2);
    char buff[256];
    esp_err_t err = esp_http_client_get_header(client, key, &buff);
    if (err == ESP_OK) {
        lua_pushstring(L, buff);
        return 1;
    }
    return 0;
}

static int l_esphttp_is_done(lua_State* L) {
    if (!lua_isuserdata(L, 1)) {
        LLOGW("check your client , which is init by esphttp.init");
        lua_pushboolean(L, 0);
        return 1;
    }

    esphttpc_t* httpc = lua_touserdata(L, 1);
    esp_http_client_handle_t client = httpc->client;

    if (client == NULL) {
        LLOGW("check your client , which is init by esphttp.init");
        lua_pushboolean(L, 0);
        return 1;
    }
    int event_id = luaL_checkinteger(L, 2);
    if (event_id == HTTP_EVENT_ON_FINISH || event_id == HTTP_EVENT_DISCONNECTED || event_id == HTTP_EVENT_ERROR) {
        lua_pushboolean(L, 1);
    }
    else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

static int l_esphttp_go(lua_State* L) {
    esphttpc_t* httpc = lua_touserdata(L, 1);
    esp_http_client_handle_t client = httpc->client;
    int timeout = luaL_checkinteger(L, 2);
    uint64_t id = luat_pushcwait(L);
    httpc->cwait_topic_id = id;

    if (httpc->started == 0) {
        httpc->started = 1;
        int ret = xTaskCreate(esphttp_client_perform_t, "esphttp", 8*1024, client, 20, NULL);
        if (ret != pdPASS) {
            LLOGW("start httpc FAIL!!! ret %d", ret);
            return 0;
        }
    }
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
    { "set_header", l_esp_http_client_set_header, 0},
    { "get_header", l_esp_http_client_get_header, 0},
    { "cleanup", l_esphttp_cleanup, 0},
    { "is_done", l_esphttp_is_done, 0},
    { "go",      l_esphttp_go, 0},

    // METHODS
    { "GET", NULL, HTTP_METHOD_GET},
    { "POST", NULL, HTTP_METHOD_POST},
    { "PUT", NULL, HTTP_METHOD_PUT},
    { "PATCH", NULL, HTTP_METHOD_PATCH},
    { "DELETE", NULL, HTTP_METHOD_DELETE},

    { "EVENT_ON_FINISH", NULL, HTTP_EVENT_ON_FINISH},
    { "EVENT_ERROR", NULL, HTTP_EVENT_ERROR},
    { "EVENT_DISCONNECTED", NULL, HTTP_EVENT_DISCONNECTED},
    { "EVENT_ON_DATA", NULL, HTTP_EVENT_ON_DATA},

	{ NULL,          NULL ,          0}
};

LUAMOD_API int luaopen_esphttp( lua_State *L ) {
    luat_newlib(L, reg_esphttp);
    return 1;
}

