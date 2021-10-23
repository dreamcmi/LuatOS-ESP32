#include "luat_base.h"
#include "luat_adc.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "sdkconfig.h"

static esp_adc_cal_characteristics_t *adc_chars;

int luat_adc_open(int pin, void *args)
{
    esp_err_t err;
    adc1_config_width(ADC_WIDTH_BIT_12);
    if (adc_chars == NULL)
        adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1,ADC_ATTEN_DB_11,ADC_WIDTH_BIT_12,0,adc_chars);

    err = adc1_config_channel_atten(pin, ADC_ATTEN_DB_11);
    if (err == ESP_OK)
    {
        return 0;
    }
    return -1;
}

int luat_adc_read(int pin, int *val, int *val2)
{
    
    uint32_t voltage,adcr;
    adcr = adc1_get_raw(pin);
    voltage =  esp_adc_cal_raw_to_voltage(adcr, adc_chars);
    *val = adcr;
    *val2 = voltage;
    return 0;
}

int luat_adc_close(int pin)
{
    return 0;
}
