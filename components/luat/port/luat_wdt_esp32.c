#include "luat_base.h"
#include "luat_wdt.h"

#include "esp_task_wdt.h"

extern TaskHandle_t luatosHandle;
int luat_wdt_init(size_t timeout)
{
    esp_err_t err = esp_task_wdt_init((timeout / 1000), true);
    if (err != 0)
    {
        return err;
    }
    return esp_task_wdt_add(luatosHandle);
}

int luat_wdt_feed(void)
{
    return esp_task_wdt_reset();
}

int luat_wdt_set_timeout(size_t timeout)
{
    return 0;
}

int luat_wdt_close(void)
{
    esp_err_t err = esp_task_wdt_delete(luatosHandle);
    if (err != 0)
    {
        return err;
    }
    return esp_task_wdt_deinit();
}