/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"

#include "mp3dec.h"
#include "string.h"

#include "driver/i2s.h"
#include "esp_system.h"
// #include "esp_check.h"
#include "esp_err.h"

#include "pinmap.h"

#define LUAT_LOG_TAG "i2s"
#include "luat_log.h"

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
    lua_pushboolean(L, err == 0);
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
    lua_pushboolean(L, err == 0);
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

// typedef struct
// {
//     char header[3];   /*!< Always "TAG" */
//     char title[30];   /*!< Audio title */
//     char artist[30];  /*!< Audio artist */
//     char album[30];   /*!< Album name */
//     char year[4];     /*!< Char array of year */
//     char comment[30]; /*!< Extra comment */
//     char genre;       /*!< See "https://en.wikipedia.org/wiki/ID3" */
// } __attribute__((packed)) mp3_id3_header_v1_t;

typedef struct
{
    char header[3]; /*!< Always "ID3" */
    char ver;       /*!< Version, equals to3 if ID3V2.3 */
    char revision;  /*!< Revision, should be 0 */
    char flag;      /*!< Flag byte, use Bit[7..5] only */
    char size[4];   /*!< TAG size */
} __attribute__((packed)) mp3_id3_header_v2_t;

typedef struct
{
    i2s_port_t port;
    const char *path;
} play_mp3_handle_t;

static QueueHandle_t audio_event_queue = NULL;
static uint8_t AUDIO_EVENT_PAUSE = 1;
static void play_mp3(void *handle)
{
    int sample_rate = 0;
    uint8_t audio_event = 0;
    MP3FrameInfo frame_info = {0};
    uint8_t *read_buf = NULL;
    uint8_t *output = NULL;
    HMP3Decoder mp3_decoder = MP3InitDecoder();
    play_mp3_handle_t *h = (play_mp3_handle_t *)handle;

    FILE *fp = fopen(h->path, "rb");
    if (fp == NULL)
    {
        LLOGE("MP3PLAYER:NO FILE");
        goto clean_up;
    }

    read_buf = malloc(MAINBUF_SIZE);
    if (read_buf == NULL)
    {
        LLOGE("MP3PLAYER:Make Read Buff But memory not enough!");
        goto clean_up;
    };

    output = malloc(1152 * sizeof(int16_t) * 2);
    if (output == NULL)
    {
        LLOGE("MP3PLAYER:Make Output Buff But memory not enough!");
        goto clean_up;
    };

    if (mp3_decoder == NULL)
    {
        LLOGE("MP3PLAYER:Init MP3Decoder But memory not enough!");
        goto clean_up;
    }

    /* Get ID3V2 head */
    mp3_id3_header_v2_t tag = {0};
    if (sizeof(mp3_id3_header_v2_t) == fread(&tag, 1, sizeof(mp3_id3_header_v2_t), fp))
    {
        if (memcmp("ID3", (const void *)&tag, sizeof(tag.header)) == 0)
        {
            int tag_len =
                ((tag.size[0] & 0x7F) << 21) +
                ((tag.size[1] & 0x7F) << 14) +
                ((tag.size[2] & 0x7F) << 7) +
                ((tag.size[3] & 0x7F) << 0);
            fseek(fp, tag_len - sizeof(mp3_id3_header_v2_t), SEEK_SET);
        }
        else
        {
            /* Not ID3V2 header */
            fseek(fp, 0, SEEK_SET);
        }
    }

    /* Start MP3 decoding */
    int bytes_left = 0;
    unsigned char *read_ptr = read_buf;
    i2s_zero_dma_buffer(h->port);

    do
    {
        if (pdPASS == xQueueReceive(audio_event_queue, &audio_event, 0))
        {
            if (AUDIO_EVENT_PAUSE == audio_event)
            {
                i2s_zero_dma_buffer(I2S_NUM_0);
                xQueuePeek(audio_event_queue, &audio_event, portMAX_DELAY);
                continue;
            }
        }

        /* Read `mainDataBegin` size to RAM */
        if (bytes_left < MAINBUF_SIZE)
        {
            memmove(read_buf, read_ptr, bytes_left);
            size_t bytes_read = fread(read_buf + bytes_left, 1, MAINBUF_SIZE - bytes_left, fp);
            bytes_left = bytes_left + bytes_read;
            read_ptr = read_buf;
        }

        /* Find MP3 sync word from read buffer */
        int offset = MP3FindSyncWord(read_buf, MAINBUF_SIZE);

        if (offset >= 0)
        {
            read_ptr += offset;   /* Data start point */
            bytes_left -= offset; /* In buffer */
            int mp3_dec_err = MP3Decode(mp3_decoder, &read_ptr, &bytes_left, (int16_t *)output, 0);
            if (mp3_dec_err != ERR_MP3_NONE)
            {
                LLOGE("MP3PLAYER:Can't decode MP3 frame:%d", mp3_dec_err);
                goto clean_up;
            }

            MP3GetLastFrameInfo(mp3_decoder, &frame_info);
            if (sample_rate != frame_info.samprate)
            {
                sample_rate = frame_info.samprate;
                uint32_t bits_cfg = frame_info.bitsPerSample;
                i2s_channel_t channel = (frame_info.nChans == 1) ? I2S_CHANNEL_MONO : I2S_CHANNEL_STEREO;
                i2s_set_clk(h->port, sample_rate, bits_cfg, channel);
            }

            size_t i2s_bytes_written = 0;
            size_t output_size = frame_info.outputSamps * frame_info.nChans;
            i2s_write(h->port, output, output_size, &i2s_bytes_written, portMAX_DELAY);
        }
        else
        {
            LLOGE("MP3PLAYER:MP3 sync word not found!");
            bytes_left = 0;
            continue;
        }
    } while (true);

clean_up:
    i2s_zero_dma_buffer(h->port);
    if (NULL != mp3_decoder)
        MP3FreeDecoder(mp3_decoder);
    if (NULL != fp)
        fclose(fp);
    if (NULL != read_buf)
        free(read_buf);
    if (NULL != output)
        free(output);
    vTaskDelete(NULL);
}

/*
mp3播放
@api i2s.mp3player(id,path)
@int i2s id
@string mp3文件路径
@return boolean 成功为true
@usage
i2s.mp3playerStart(0,"/spiffs/test.mp3")
*/
static TaskHandle_t mp3PlayerHandle = NULL;
static int l_i2s_mp3player_start(lua_State *L)
{
    play_mp3_handle_t handle = {0};
    handle.port = luaL_optinteger(L, 1, 0);
    handle.path = luaL_checkstring(L, 2);

    audio_event_queue = xQueueCreate(4, sizeof(uint8_t));
    if (audio_event_queue == NULL)
    {
        LLOGE("audio_event_queue init error");
        lua_pushboolean(L, false);
        return 1;
    }
    BaseType_t re = xTaskCreate(play_mp3, "play_mp3", 4096, &handle, 10, &mp3PlayerHandle);
    lua_pushboolean(L, re == pdPASS);
    return 1;
}

/*
mp3播放暂停
@api i2s.mp3playerPause()
@return boolean 成功为true
@usage
i2s.mp3playerPause()
*/
static int l_i2s_mp3player_pause(lua_State *L)
{
    uint8_t event = AUDIO_EVENT_PAUSE;
    BaseType_t re = xQueueSend(audio_event_queue, &event, 0);
    lua_pushboolean(L, re == pdPASS);
    return 1;
}

/*
mp3播放终止
@api i2s.mp3playerStop()
@return boolean 成功为true
@usage
i2s.mp3playerStop()
*/
static int l_i2s_mp3player_stop(lua_State *L)
{
    i2s_zero_dma_buffer(I2S_NUM_0);
    vTaskDelete(mp3PlayerHandle);
    vQueueDelete(audio_event_queue);
    lua_pushboolean(L, true);
    return 1;
}

#include "rotable.h"
static const rotable_Reg reg_i2s[] =
    {
        {"setup", l_i2s_setup, 0},
        {"close", l_i2s_close, 0},
        {"send", l_i2s_send, 0},
        {"recv", l_i2s_recv, 0},
        {"mp3playerStart", l_i2s_mp3player_start, 0},
        {"mp3playerPause", l_i2s_mp3player_pause, 0},
        {"mp3playerStop", l_i2s_mp3player_stop, 0},

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
