/*
 * SPDX-FileCopyrightText: 2021-2022 Wendal <wendal1985@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"
#include "luat_msgbus.h"
#include "luat_malloc.h"

/*
@module  espmqtt
@summary mqtt/websocket客户端
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
#include "mqtt_client.h"

#define LUAT_LOG_TAG "espmqtt"
#include "luat_log.h"

typedef struct mqtt_data {
    char* topic;
    char* data;
    size_t topic_len;
    size_t data_len;
}mqtt_data_t;

static inline esp_mqtt_client_handle_t _mqttc(lua_State *L) {
    if (!lua_islightuserdata(L, 1)) {
        LLOGW("check your client , which is init by espmqtt.init");
        return NULL;
    }
    esp_mqtt_client_handle_t client = lua_touserdata(L, 1);
    if (client == NULL) {
        LLOGW("check your client , which is init by espmqtt.init");
        return NULL;
    }
    return client;
}

static int l_mqtt_event_cb(lua_State *L, void* ptr) {
    //esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)ptr;
    rtos_msg_t* msg = (rtos_msg_t*)lua_topointer(L, -1);
    lua_getglobal(L, "sys_pub");
    if (lua_isfunction(L, -1)) {
        lua_pushliteral(L, "ESPMQTT_EVT");
        lua_pushlightuserdata(L, ptr);
        lua_pushinteger(L, msg->arg1);
        LLOGD("l_mqtt_event_cb %d", msg->arg1);
        if (msg->arg1 == MQTT_EVENT_DATA) {
            mqtt_data_t * mdata = (mqtt_data_t *)msg->arg2;
            if (mdata) {
                lua_pushlstring(L, mdata->topic, mdata->topic_len);
                lua_pushlstring(L, mdata->data, mdata->data_len);
                lua_call(L, 5, 0);
            
                if (mdata->topic)
                    luat_heap_free(mdata->topic);
                if (mdata->data)
                    luat_heap_free(mdata->data);
                luat_heap_free(mdata);
            }
        }
        else {
            // LLOGD("%s %d >>2", __FILE__, __LINE__);
            lua_call(L, 3, 0);
        }
    }
    // LLOGD("%s %d >>3", __FILE__, __LINE__);
    return 0;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    LLOGD("event %d %p", event_id, client);
    rtos_msg_t msg = {0};
    msg.handler = l_mqtt_event_cb;
    msg.ptr = client;
    msg.arg1 = (esp_mqtt_event_id_t)event_id;
    if ((esp_mqtt_event_id_t)event_id == MQTT_EVENT_DATA) {
        mqtt_data_t * mdata = luat_heap_malloc(sizeof(mqtt_data_t));
        if (mdata == NULL) {
            LLOGD("out of memory ?!!!");
            return;
        }
        mdata->topic_len = event->topic_len;
        if (event->topic_len) {
            mdata->topic = luat_heap_malloc(event->topic_len);
            memcpy(mdata->topic, event->topic, event->topic_len);
        }
        mdata->data_len = event->data_len;
        if (event->data_len) {
            mdata->data = luat_heap_malloc(event->data_len);
            memcpy(mdata->data, event->data, event->data_len);
        }
        msg.arg2 = (uint32_t)mdata;
    }
    // LLOGD("event %d %p end", event_id, event->client);
    luat_msgbus_put(&msg, 0);
    // LLOGD("event %d %p end2", event_id, event->client);
    return;
}

/*
初始化mqtt/websocket客户端
@api espmqtt.init(params)
@table 参数表,包括: uri,username,password,client_id,keepalive等
@return userdata 参数ok返回句柄, 否则返回nil
@usage
-- 创建一个mqttc实例, 详细用法请查阅 demo/espmqtt
local mqttc = espmqtt.init({
    uri = "mqtt://lbsmqtt.airm2m.com:1884",
    client_id = (esp32.getmac():toHex())
})
log.info("mqttc", "client", mqttc)
*/
static int l_espmqtt_init(lua_State* L) {
    esp_mqtt_client_config_t mqtt_cfg;
    memset(&mqtt_cfg, 0, sizeof(esp_mqtt_client_config_t));
    // mqtt_cfg.event_handle = mqtt_event_handler;
    if (!lua_istable(L, 1)) {
        lua_pushliteral(L, "mqtt init need table");
        lua_error(L);
        return 0;
    }
    //lua_settop(L, 1);

    // 读取配置信息----------------------
    LLOGD("read config ...");
    // uri
    if (lua_getfield(L, 1, "uri") == LUA_TSTRING) {
        mqtt_cfg.uri = luaL_checkstring(L, -1);
        LLOGD("uri %s", mqtt_cfg.uri);
    }
    lua_pop(L, 1);

    // host
    if (lua_getfield(L, 1, "host") == LUA_TSTRING) {
        mqtt_cfg.host = luaL_checkstring(L, -1);
    }
    lua_pop(L, 1);

    // port
    if (lua_getfield(L, 1, "port") == LUA_TNUMBER) {
        mqtt_cfg.port = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    // client_id
    if (lua_getfield(L, 1, "client_id") == LUA_TSTRING) {
        mqtt_cfg.client_id = luaL_checkstring(L, -1);
    }
    lua_pop(L, 1);

    // username
    if (lua_getfield(L, 1, "username") == LUA_TSTRING) {
        mqtt_cfg.username = luaL_checkstring(L, -1);
    }
    lua_pop(L, 1);

    // password
    if (lua_getfield(L, 1, "password") == LUA_TSTRING) {
        mqtt_cfg.password = luaL_checkstring(L, -1);
    }
    lua_pop(L, 1);

    // keepalive
    if (lua_getfield(L, 1, "keepalive") == LUA_TNUMBER) {
        mqtt_cfg.keepalive = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    // path
    if (lua_getfield(L, 1, "path") == LUA_TSTRING) {
        mqtt_cfg.path = luaL_checkstring(L, -1);
    }
    lua_pop(L, 1);

    // 暂不支持遗嘱消息
    // // lwt_topic
    // lua_getfield(L, 1, "lwt_topic");
    // if (lua_isstring(L, 1)) {
    //     mqtt_cfg.lwt_topic = luaL_checkstring(L, -1);
    // }
    // lua_pop(L, 1);

    // // lwt_msg
    // lua_getfield(L, 1, "lwt_msg");
    // if (lua_isstring(L, 1)) {
    //     mqtt_cfg.lwt_msg = luaL_checkstring(L, -1);
    // }
    // lua_pop(L, 1);

    // -------------------------------------

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    if (client != NULL) {
        esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
        lua_pushlightuserdata(L, client);
        return 1;
    }
    return 0;
}

/*
启动客户端,连接到服务器
@api espmqtt.start(client)
@userdata 通过espmqtt.init创建的句柄
@return bool 成功返回true,否则返回false
*/
static int l_espmqtt_start(lua_State* L) {
    esp_mqtt_client_handle_t client = _mqttc(L);
    if (client == NULL) return 0; 
    int ret = esp_mqtt_client_start(client);
    lua_pushboolean(L, ret == ESP_OK ? 1 : 0);
    lua_pushinteger(L, ret);
    return 2;
}

/*
停止客户端,断开与服务器的连接
@api espmqtt.stop(client)
@userdata 通过espmqtt.init创建的句柄
@return bool 成功返回true,否则返回false
*/
static int l_espmqtt_stop(lua_State* L) {
    esp_mqtt_client_handle_t client = _mqttc(L);
    if (client == NULL) return 0; 
    int ret = esp_mqtt_client_stop(client);
    lua_pushboolean(L, ret == ESP_OK ? 1 : 0);
    lua_pushinteger(L, ret);
    return 2;
}

/*
销毁客户端,清理相关资源
@api espmqtt.stop(client)
@userdata 通过espmqtt.init创建的句柄
@return bool 成功返回true,否则返回false
*/
static int l_espmqtt_destroy(lua_State* L) {
    esp_mqtt_client_handle_t client = _mqttc(L);
    if (client == NULL) return 0; 
    int ret = esp_mqtt_client_destroy(client);
    lua_pushboolean(L, ret == ESP_OK ? 1 : 0);
    lua_pushinteger(L, ret);
    return 2;
}

/*
订阅一个主题
@api espmqtt.subscribe(client, topic, qos)
@userdata 通过espmqtt.init创建的句柄
@string 主题, 必须填写
@int 级别,默认为0,可选
@return bool 成功返回true,否则返回false
*/
static int l_espmqtt_subscribe(lua_State* L) {
    esp_mqtt_client_handle_t client = _mqttc(L);
    if (client == NULL) return 0; 
    const char* topic = luaL_checkstring(L, 2);
    int qos = luaL_optinteger(L, 3, 0);
    int ret = esp_mqtt_client_subscribe(client, topic, qos);
    lua_pushboolean(L, ret == ESP_OK ? 1 : 0);
    lua_pushinteger(L, ret);
    return 2;
}

/*
取消订阅一个主题
@api espmqtt.unsubscribe(client, topic)
@userdata 通过espmqtt.init创建的句柄
@string 主题, 必须填写
@return bool 成功返回true,否则返回false
*/
static int l_espmqtt_unsubscribe(lua_State* L) {
    esp_mqtt_client_handle_t client = _mqttc(L);
    if (client == NULL) return 0; 
    const char* topic = luaL_checkstring(L, 2);
    int ret = esp_mqtt_client_unsubscribe(client, topic);
    lua_pushboolean(L, ret == ESP_OK ? 1 : 0);
    lua_pushinteger(L, ret);
    return 2;
}

/*
往主题发送一个消息
@api espmqtt.publish(client, topic, data, qos, retain)
@userdata 通过espmqtt.init创建的句柄
@string 主题, 必须填写
@string 数据, 必须填写
@int 级别,默认为0,可选
@int 默认为0
@return bool 成功返回true,否则返回false
*/
static int l_espmqtt_publish(lua_State* L) {
    esp_mqtt_client_handle_t client = _mqttc(L);
    if (client == NULL) return 0; 
    const char* topic = luaL_checkstring(L, 2);
    size_t data_len;
    const char* data = luaL_checklstring(L, 3, &data_len);
    int qos = luaL_optinteger(L, 4, 0);
    int retain = luaL_optinteger(L, 5, 0);
    int msgid = esp_mqtt_client_publish(client, topic, data, data_len, qos, retain);
    lua_pushboolean(L, msgid != -1 ? 1 : 0);
    lua_pushinteger(L, msgid);
    return 2;
}

#include "rotable.h"
static const rotable_Reg reg_espmqtt[] =
{
	{ "init",          l_espmqtt_init ,          0},
    { "start",         l_espmqtt_start,          0},
    { "subscribe",     l_espmqtt_subscribe,      0},
    { "unsubscribe",   l_espmqtt_unsubscribe,    0},
    { "publish",       l_espmqtt_publish,        0},
	{ "stop",          l_espmqtt_stop ,          0},
    { "destroy",       l_espmqtt_destroy,        0},

    { "EVENT_ERROR", NULL, MQTT_EVENT_ERROR},
    { "EVENT_CONNECTED", NULL, MQTT_EVENT_CONNECTED},
    { "EVENT_DISCONNECTED", NULL, MQTT_EVENT_DISCONNECTED},
    { "EVENT_SUBSCRIBED", NULL, MQTT_EVENT_SUBSCRIBED},
    { "EVENT_UNSUBSCRIBED", NULL, MQTT_EVENT_UNSUBSCRIBED},
    { "EVENT_PUBLISHED", NULL, MQTT_EVENT_PUBLISHED},
    { "EVENT_DATA", NULL, MQTT_EVENT_DATA},
	{ NULL,          NULL ,          0}
};

LUAMOD_API int luaopen_espmqtt( lua_State *L ) {
    luat_newlib(L, reg_espmqtt);
    return 1;
}

