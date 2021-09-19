#include "luat_base.h"
#include "luat_pwm.h"

#define LUAT_LOG_TAG "luat.pwm"
#include "luat_log.h"
#include "driver/ledc.h"
#include "esp_err.h"

uint32_t map(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int luat_pwm_open(int channel, size_t period, size_t pulse)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = period,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = channel,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    }; 
    ledc_channel_config(&ledc_channel);

    ledc_fade_func_install(0);
    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE,ledc_channel.channel, map(pulse,0,100,0,8191),10);

    return 0;
}
int luat_pwm_close(int channel)
{
    ledc_stop(LEDC_LOW_SPEED_MODE,channel,0);
    return 0;
}