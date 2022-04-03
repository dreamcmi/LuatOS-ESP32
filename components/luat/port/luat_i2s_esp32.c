/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"

#include "driver/i2s.h"
#include "esp_system.h"
// #include "esp_check.h"
#include "esp_err.h"

#include "pinmap.h"

/*
i2s初始化
@api i2s.setup(id,rate,bit,channel_format,communication_format,mclk_multiple)
@int i2sid
@int i2s采样率 默认16k 可选8/16/24/32
@int 1个channel对应的位数 默认16bit
@int 通道设置 默认左右声道 可选i2s.RLCH(左右声道) i2s.ARCH(全右声道) i2s.ALCH(全左声道) i2s.ORCH(单右通道) i2s.OLCH(单左通道)
@int i2s通讯格式 默认i2s 可选i2s.STAND_I2S i2s.STAND_MSB i2s.STAND_PCM_SHORT i2s.STAND_PCM_LONG
@int mclk频率 默认sample_rate * 256 可选128/256/384
@return int esp_err_t
@usage
i2s.setup(0,48*1000)
*/
static int l_i2s_setup(lua_State *L)
{
    esp_err_t err = -1;
    int i2s_num = luaL_checkinteger(L, 1);
    if (i2s_num >= I2S_NUM_MAX)
    {
        lua_pushinteger(L, -1);
        return 1;
    }
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX,
        .sample_rate = luaL_optinteger(L, 2, 16000),
        .bits_per_sample = luaL_optinteger(L, 3, I2S_BITS_PER_SAMPLE_16BIT),
        .channel_format = luaL_optinteger(L, 4, I2S_CHANNEL_FMT_RIGHT_LEFT),
        .communication_format = luaL_optinteger(L, 5, I2S_COMM_FORMAT_STAND_I2S),
        .mclk_multiple = luaL_optinteger(L, 6, I2S_MCLK_MULTIPLE_DEFAULT),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 6,
        .dma_buf_len = 160,
        .use_apll = false,
        .tx_desc_auto_clear = true};
    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
#if CONFIG_IDF_TARGET_ESP32C3
    i2s_pin_config_t pin_config = {
        .bck_io_num = _C3_I2S0_SCLK,
        .ws_io_num = _C3_I2S0_WS,
        .data_out_num = _C3_I2S0_DO,
        .data_in_num = _C3_I2S0_DI,
        .mck_io_num = _C3_I2S0_MCLK,
    };
#elif CONFIG_IDF_TARGET_ESP32S3
    i2s_pin_config_t pin_config = {
        .bck_io_num = _S3_I2S0_SCLK,
        .ws_io_num = _S3_I2S0_WS,
        .data_out_num = _S3_I2S0_DO,
        .data_in_num = _S3_I2S0_DI,
        .mck_io_num = _S3_I2S0_MCLK,
    };
#else
#error "I2S SETUP ERROR Please make sure the target is esp32c3 or esp32s3"
#endif
    err = i2s_set_pin(i2s_num, &pin_config);
    lua_pushinteger(L, err);
    return 1;
}

/*
i2s去初始化
@api i2s.close(id)
@int i2sid
@return int esp_err_t
@usage
i2s.close(0)
*/
static int l_i2s_close(lua_State *L)
{
    esp_err_t err = -1;
    int i2s_num = luaL_checkinteger(L, 1);
    if (i2s_num >= I2S_NUM_MAX)
    {
        lua_pushinteger(L, -1);
        return 1;
    }
    i2s_stop(i2s_num);
    err = i2s_driver_uninstall(i2s_num);
    lua_pushinteger(L, err);
    return 1;
}

/*
i2s写
@api i2s.send(id,buff)
@int i2sid
@string buff
@return int esp_err_t
@usage
i2s.send(0,string.fromHex("ff"))
*/
static int l_i2s_send(lua_State *L)
{
    esp_err_t err = -1;
    size_t bytes_write = 0;
    size_t len = 0;

    int i2s_num = luaL_checkinteger(L, 1);
    if (i2s_num >= I2S_NUM_MAX)
    {
        lua_pushinteger(L, -1);
        return 1;
    }
    const char *sdata = luaL_checklstring(L, 2, &len);
    err = i2s_write(i2s_num, sdata, len, &bytes_write, 100 / portTICK_RATE_MS);
    i2s_zero_dma_buffer(i2s_num);
    if (bytes_write < len)
    {
        lua_pushinteger(L, ESP_ERR_TIMEOUT); // if timeout, bytes_write < len.
    }
    else
    {
        lua_pushinteger(L, err);
    }
    return 1;
}

/*
i2s读
@api i2s.send(id,len)
@int i2sid
@int 读长度,默认1024
@return int esp_err_t
@usage
i2s.send(0,256)
*/
static int l_i2s_recv(lua_State *L)
{
    esp_err_t err = -1;
    size_t bytes_read = 0;

    int i2s_num = luaL_checkinteger(L, 1);
    if (i2s_num >= I2S_NUM_MAX)
    {
        lua_pushinteger(L, -1);
        return 1;
    }
    int rlen = luaL_optinteger(L, 2, 1024);
    char *mic_data = (char *)calloc(rlen, sizeof(char));
    if (mic_data == NULL)
    {
        lua_pushnil(L);
        return 1;
    }
    err = i2s_read(i2s_num, mic_data, rlen, &bytes_read, 100 / portTICK_RATE_MS);
    if (err == 0)
    {
        lua_pushlstring(L, mic_data, rlen);
    }
    else
    {
        lua_pushnil(L);
    }
    free(mic_data);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_i2s[] =
    {
        {"setup", l_i2s_setup, 0},
        {"close", l_i2s_close, 0},
        {"send", l_i2s_send, 0},
        {"recv", l_i2s_recv, 0},

        {"RLCH", NULL, I2S_CHANNEL_FMT_RIGHT_LEFT},
        {"ARCH", NULL, I2S_CHANNEL_FMT_ALL_RIGHT},
        {"ALCH", NULL, I2S_CHANNEL_FMT_ALL_LEFT},
        {"ORCH", NULL, I2S_CHANNEL_FMT_ONLY_RIGHT},
        {"OLCH", NULL, I2S_CHANNEL_FMT_ONLY_LEFT},
        {"STAND_I2S", NULL, I2S_COMM_FORMAT_STAND_I2S},
        {"STAND_MSB", NULL, I2S_COMM_FORMAT_STAND_MSB},
        {"STAND_PCM_SHORT", NULL, I2S_COMM_FORMAT_STAND_PCM_SHORT},
        {"STAND_PCM_LONG", NULL, I2S_COMM_FORMAT_STAND_PCM_LONG},

        {NULL, NULL, 0}};

LUAMOD_API int luaopen_i2s(lua_State *L)
{
    luat_newlib(L, reg_i2s);
    return 1;
}
