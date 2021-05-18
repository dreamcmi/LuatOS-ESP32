#include "luat_base.h"
#include "luat_pwm.h"

#define LUAT_LOG_TAG "luat.pwm"
#include "luat_log.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

int luat_pwm_open(int channel, size_t period, size_t pulse);
int luat_pwm_close(int channel);