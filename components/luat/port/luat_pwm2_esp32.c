/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  pwm2
@summary esp32专用pwm2库
@version 1.0
@date    2022.2.15
*/
#include "luat_base.h"
#define LUAT_LOG_TAG "luat.pwm"
#include "luat_log.h"

#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "driver/ledc.h"
#include "esp_err.h"

/*
pwm2初始化
@api pwm2.init(pwm_timer,pwm_channel,pwm_pin,pwm_bit,pwm_period,pwm_fade)
@int pwm_timer   定时器id 取值0~3
@int pwm_channel 通道id 取值0~5
@int pwm_pin     引脚
@int pwm_bit     分辨率 取值1~13
@int pwm_period  频率
@int pwm_fade    渐入渐出服务
@return int pwm_channel
@usage pwm2.init(0,0,1,10,1*1000,0)
*/
static int l_pwm2_init(lua_State *L)
{
    // esp_err_t err = -1;
    int pwm_timer = luaL_checkinteger(L, 1);
    int pwm_channel = luaL_checkinteger(L, 2);
    int pwm_pin = luaL_checkinteger(L, 3);
    int pwm_bit = luaL_checkinteger(L, 4);
    int pwm_period = luaL_checkinteger(L, 5);
    int pwm_fade = luaL_optinteger(L, 6, 0); // 默认不开渐入渐出

    ledc_timer_config_t conf = {0};
    conf.speed_mode = LEDC_LOW_SPEED_MODE;
    conf.duty_resolution = pwm_bit,
    conf.timer_num = pwm_timer,
    conf.freq_hz = pwm_period,
    conf.clk_cfg = LEDC_AUTO_CLK,
    ledc_timer_config(&conf);

    ledc_channel_config_t ledc_channel = {0};
    ledc_channel.gpio_num = pwm_pin,
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE,
    ledc_channel.channel = pwm_channel,
    ledc_channel.timer_sel = pwm_timer,
    ledc_channel.duty = 0,
    ledc_channel.hpoint = 0,
    ledc_channel_config(&ledc_channel);

    if (pwm_fade)
    {
        ledc_fade_func_install(0);
    }

    lua_pushinteger(L, pwm_channel);
    return 1;
}

/*
pwm2调节占空比
@api pwm2.setDuty(pwm_channel,pwm_duty)
@int pwm_channel 通道id 取值0~5
@int pwm_duty    占空比
@return esp_err_t
@usage 
pwm2.setDuty(0,512)
*/
static int l_pwm2_set_duty(lua_State *L)
{
    esp_err_t err = -1;
    int pwm_channel = luaL_checkinteger(L, 1);
    int pwm_duty = luaL_checkinteger(L, 2);

    err = ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, pwm_duty);
    //todo,ESP_ERROR_CHECK(err);
    err = ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel);
    //todo,ESP_ERROR_CHECK(err);

    lua_pushinteger(L, err);
    return 1;
}

/*
pwm2渐入渐出定时
@api pwm2.setFadeWithTime(pwm_channel,pwm_duty,pwm_ms,pwm_wait)
@int pwm_channel 通道id 取值0~5
@int pwm_duty    占空比
@int pwm_ms      到达目标占空比的时间(ms)
@int pwm_wait    默认为1(阻塞)
@return esp_err_t
@usage
pwm2.setFadeWithTime(0,512,10)
*/
static int l_pwm2_set_fade_with_time(lua_State *L)
{
    esp_err_t err = -1;
    int pwm_channel = luaL_checkinteger(L, 1);
    int pwm_duty = luaL_checkinteger(L, 2);
    int pwm_ms = luaL_checkinteger(L, 3);
    int pwm_wait = luaL_optinteger(L, 4, 1);

    err = ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, pwm_channel, pwm_duty, pwm_ms);
    //todo,ESP_ERROR_CHECK(err);
    err = ledc_fade_start(LEDC_LOW_SPEED_MODE, pwm_channel, pwm_wait);
    //todo,ESP_ERROR_CHECK(err);

    lua_pushinteger(L, err);
    return 1;
}

/*
pwm2获取定时器频率
@api pwm2.getFreq(pwm_timer)
@int pwm_timer 定时器
@return int 频率
@usage
freq = pwm2.getFreq(0)
*/
static int l_pwm2_get_freq(lua_State *L)
{
    int pwm_timer = luaL_checkinteger(L, 1);

    uint32_t hz = ledc_get_freq(LEDC_LOW_SPEED_MODE, pwm_timer);
    lua_pushinteger(L, hz);
    return 1;
}

/*
pwm2卸载渐入渐出服务
@api pwm2.uninstallFade()
@usage
pwm2.uninstallFade()
*/
static int l_pwm2_uninstall_fade(lua_State *L)
{
    ledc_fade_func_uninstall();
    return 0;
}

#include "rotable.h"
static const rotable_Reg reg_pwm2[] =
    {
        {"init", l_pwm2_init, 0},
        {"setDuty", l_pwm2_set_duty, 0},
        {"setFadeWithTime", l_pwm2_set_fade_with_time, 0},
        {"getFreq", l_pwm2_get_freq, 0},
        {"uninstallFade", l_pwm2_uninstall_fade, 0},
        {NULL, NULL, 0}};

LUAMOD_API int luaopen_pwm2(lua_State *L)
{
    luat_newlib(L, reg_pwm2);
    return 1;
}
