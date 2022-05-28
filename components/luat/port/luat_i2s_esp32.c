/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "string.h"

#include "luat_base.h"
#include "luat_fs.h"
#include "luat_i2s.h"
#define LUAT_LOG_TAG "i2s"
#include "luat_log.h"

#include "driver/i2s.h"
#include "esp_system.h"
#include "esp_err.h"

#include "pinmap.h"
#include "mp3dec.h"

int luat_i2s_setup(luat_i2s_conf_t *conf)
{
    i2s_config_t i2s_config = {
        .sample_rate = conf->sample_rate,
        .bits_per_sample = conf->bits_per_sample,
        .mclk_multiple = conf->mclk,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 6,
        .dma_buf_len = 160,
        .use_apll = false,
        .tx_desc_auto_clear = true};
    if (conf->mode == 0)
    {
        i2s_config.mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX;
    }
    else
    {
        LLOGE("NOT SUPPORT I2S MODE:%d", conf->mode);
        return ESP_ERR_INVALID_ARG;
    }

    if (conf->channel_format == 0) // 左声道
    {
        i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    }
    else if (conf->channel_format == 1) // 右声道
    {
        i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT;
    }
    else if (conf->channel_format == 2) // 左右声道
    {
        i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    }
    else
    {
        LLOGE("NOT SUPPORT I2S CHANNEL:%d", conf->channel_format);
        return ESP_ERR_INVALID_ARG;
    }

    if (conf->communication_format == 0)
    {
        i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    }
    else
    {
        LLOGE("NOT SUPPORT I2S communication_format:%d", conf->communication_format);
        return ESP_ERR_INVALID_ARG;
    }

    i2s_driver_install(conf->id, &i2s_config, 0, NULL);
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
    return i2s_set_pin(conf->id, &pin_config);
}

int luat_i2s_close(uint8_t id)
{
    i2s_stop(id);
    return i2s_driver_uninstall(id);
}

int luat_i2s_send(uint8_t id, char *buff, size_t len)
{
    esp_err_t err = -1;
    size_t bytes_write = 0;

    err = i2s_write(id, (const void *)buff, len, &bytes_write, 100 / portTICK_RATE_MS);
    i2s_zero_dma_buffer(id);
    return err;
}

int luat_i2s_recv(uint8_t id, char *buff, size_t len)
{
    size_t bytes_read = 0;
    return i2s_read(id, (void *)buff, len, &bytes_read, 100 / portTICK_RATE_MS);
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

static TaskHandle_t mp3PlayerHandle = NULL;
static TaskHandle_t mp3PlayerHandle1 = NULL;
static QueueHandle_t audio_event_queue = NULL;
static QueueHandle_t audio_event_queue1 = NULL;
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

    FILE *fp = luat_fs_fopen(h->path, "rb");
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
    if (sizeof(mp3_id3_header_v2_t) == luat_fs_fread(&tag, 1, sizeof(mp3_id3_header_v2_t), fp))
    {
        if (memcmp("ID3", (const void *)&tag, sizeof(tag.header)) == 0)
        {
            int tag_len =
                ((tag.size[0] & 0x7F) << 21) +
                ((tag.size[1] & 0x7F) << 14) +
                ((tag.size[2] & 0x7F) << 7) +
                ((tag.size[3] & 0x7F) << 0);
            luat_fs_fseek(fp, tag_len - sizeof(mp3_id3_header_v2_t), SEEK_SET);
        }
        else
        {
            /* Not ID3V2 header */
            luat_fs_fseek(fp, 0, SEEK_SET);
        }
    }

    /* Start MP3 decoding */
    int bytes_left = 0;
    unsigned char *read_ptr = read_buf;
    i2s_zero_dma_buffer(h->port);

    do
    {
        if (h->port == 0)
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
        }
        else if (h->port == 1)
        {
            if (pdPASS == xQueueReceive(audio_event_queue1, &audio_event, 0))
            {
                if (AUDIO_EVENT_PAUSE == audio_event)
                {
                    i2s_zero_dma_buffer(I2S_NUM_0);
                    xQueuePeek(audio_event_queue1, &audio_event, portMAX_DELAY);
                    continue;
                }
            }
        }

        /* Read `mainDataBegin` size to RAM */
        if (bytes_left < MAINBUF_SIZE)
        {
            memmove(read_buf, read_ptr, bytes_left);
            size_t bytes_read = luat_fs_fread(read_buf + bytes_left, 1, MAINBUF_SIZE - bytes_left, fp);
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
                i2s_zero_dma_buffer(h->port);
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

// i2s.play(0,"/spiffs/test.mp3")
int l_i2s_play(lua_State *L)
{
    BaseType_t re = pdFALSE;
    play_mp3_handle_t handle = {0};
    handle.port = luaL_optinteger(L, 1, 0);
    handle.path = luaL_checkstring(L, 2);

    if (!strstr(handle.path, ".mp3"))
    {
        if (handle.port == 0)
        {
            audio_event_queue = xQueueCreate(4, sizeof(uint8_t));
            if (audio_event_queue == NULL)
            {
                LLOGE("audio_event_queue init error");
                lua_pushboolean(L, false);
                return 1;
            }
            re = xTaskCreate(play_mp3, "play_mp3_0", 4096, &handle, 10, &mp3PlayerHandle);
        }
        else if (handle.port == 1)
        {
            audio_event_queue1 = xQueueCreate(4, sizeof(uint8_t));
            if (audio_event_queue1 == NULL)
            {
                LLOGE("audio_event_queue init error");
                lua_pushboolean(L, false);
                return 1;
            }
            re = xTaskCreate(play_mp3, "play_mp3_1", 4096, &handle, 10, &mp3PlayerHandle1);
        }

        lua_pushboolean(L, re == pdPASS);
        return 1;
    }
    else if (!strstr(handle.path, ".wav"))
    {
        // TODO WAV
        return -1;
    }
    else
    {
        return -1;
    }
}

// i2s.pause(0)
int l_i2s_pause(lua_State *L)
{
    BaseType_t re = pdFALSE;
    uint8_t event = AUDIO_EVENT_PAUSE;
    int port = luaL_optinteger(L, 1, 0);
    if (port == 0)
    {
        re = xQueueSend(audio_event_queue, &event, 0);
    }
    else if (port == 1)
    {
        re = xQueueSend(audio_event_queue1, &event, 0);
    }
    lua_pushboolean(L, re == pdPASS);
    return 1;
}

// i2s.stop(0)
int l_i2s_stop(lua_State *L)
{
    int port = luaL_optinteger(L, 1, 0);
    if (port == 0)
    {
        i2s_zero_dma_buffer(port);
        vTaskDelete(mp3PlayerHandle);
        vQueueDelete(audio_event_queue);
        lua_pushboolean(L, true);
    }
    else if (port == 1)
    {
        i2s_zero_dma_buffer(port);
        vTaskDelete(mp3PlayerHandle1);
        vQueueDelete(audio_event_queue1);
        lua_pushboolean(L, true);
    }
    else
    {
        lua_pushboolean(L, false);
    }
    return 1;
}
