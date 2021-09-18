#include "luat_base.h"
#include "luat_adc.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "sdkconfig.h"

int luat_adc_open(int pin, void *args)
{
    adc1_config_width(ADC_WIDTH_12Bit);
    esp_err_t err = adc1_config_channel_atten(pin, ADC_ATTEN_MAX);
    if (err == ESP_OK)
    {
        return 0;
    }
    return -1;
}

int luat_adc_read(int pin, int *val, int *val2)
{   
    *val = adc1_get_raw(pin);
    return 0;
}

int luat_adc_close(int pin)
{
    return 0;
}
