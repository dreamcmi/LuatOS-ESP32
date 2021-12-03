#include "luat_base.h"
#include "luat_mcu.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

long luat_mcu_ticks(void) {
    return xTaskGetTickCount();
}
