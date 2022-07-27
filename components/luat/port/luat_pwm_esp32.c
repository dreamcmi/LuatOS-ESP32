/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"
#include "luat_pwm.h"

#define LUAT_LOG_TAG "luat.pwm"
#include "luat_log.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_err.h"

// uint32_t map(int x, int in_min, int in_max, int out_min, int out_max)
// {
//     return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
// }

static uint8_t pgroup[SOC_LEDC_CHANNEL_NUM][2] = {0};
static uint8_t pwmid = 0;

int luat_pwm_setup(luat_pwm_conf_t *conf)
{
    int pc = -1;

    for (int i = 0; i < SOC_LEDC_CHANNEL_NUM; i++)
    {
        if (pgroup[i][1] == conf->channel)
        {
            pc = i;
            break;
        }
    }

    if (pc == -1 && pgroup[pwmid][0] == 0)
    {
        // printf("init%d\n", pwmid);
        ledc_timer_config_t ledc_timer = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = LEDC_TIMER_13_BIT,
            .timer_num = pwmid,
            .freq_hz = conf->period,
            .clk_cfg = LEDC_AUTO_CLK,
        };
        ledc_timer_config(&ledc_timer);
        ledc_channel_config_t ledc_channel = {
            .gpio_num = conf->channel,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = pwmid,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0,
        };
        ledc_channel_config(&ledc_channel);
        pgroup[pwmid][0] = 1;
        pgroup[pwmid][1] = conf->channel;
        pc = pwmid;
        pwmid += 1;
    }

    // printf("pin:%d:pulse:%d\n", pc, conf->pulse);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, pc, conf->pulse); //todo error check
    ledc_update_duty(LEDC_LOW_SPEED_MODE, pc); //todo error check
    return 0;
}
int luat_pwm_close(int channel)
{
    int pc = -1;
    for (int i = 0; i < SOC_LEDC_CHANNEL_NUM; i++)
    {
        if (pgroup[i][1] == channel)
        {
            pc = i;
            break;
        }
    }
    if (pc != -1)
    {
        ledc_stop(LEDC_LOW_SPEED_MODE, pc, 0);
        pgroup[pc][0] = 0;
        gpio_reset_pin(channel);
        return 0;
    }
    return -1;
}

int luat_pwm_capture(int channel, int freq)
{
    return -1;
}
