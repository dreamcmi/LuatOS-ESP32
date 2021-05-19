#include "luat_base.h"
#include "luat_pwm.h"

#define LUAT_LOG_TAG "luat.pwm"
#include "luat_log.h"
#include "driver/ledc.h"
#include "esp_err.h"

// LEDC_CHANNEL_0
// LEDC_CHANNEL_1
// LEDC_CHANNEL_2
// LEDC_CHANNEL_3
// 四个通道，后续支持

int luat_pwm_open(int channel, size_t period, size_t pulse)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,   // timer mode
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = period,                    // frequency of PWM signal
        .timer_num = LEDC_TIMER_0,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .gpio_num = channel,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = pulse,
        .hpoint = 0,
    };
    ledc_channel_config(&ledc_channel);

    return 0;
}
int luat_pwm_close(int channel)
{
    ledc_stop(LEDC_HIGH_SPEED_MODE,channel,0);
    return 0;
}