#include "luat_base.h"
#include "luat_adc.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
#elif CONFIG_IDF_TARGET_ESP32S2
static const adc_bits_width_t width = ADC_WIDTH_BIT_13;
#endif

static const adc_atten_t atten = ADC_ATTEN_DB_0;

#define DEFAULT_VREF 1100
#define NO_OF_SAMPLES 64 //Multisampling
// static const adc_unit_t unit = ADC_UNIT_1;

uint32_t l_adc1_get_voltage(adc1_channel_t channel)
{
    static esp_adc_cal_characteristics_t *adc_chars;
    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    uint32_t adc_reading = 0;
    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++)
    {
        adc_reading += adc1_get_raw(channel);
    }

    adc_reading /= NO_OF_SAMPLES;
    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    return voltage;
}


int luat_adc_open(int pin, void *args)
{
    adc1_config_width(width);
    #if CONFIG_IDF_TARGET_ESP32
    if (pin >= 32 && pin <= 39)
    {
        switch (pin)
        {
        case 32:
            adc1_config_channel_atten(ADC1_CHANNEL_4, atten);
            // adc1_config_channel_atten(ADC1_GPIO32_CHANNEL,atten); /*idf两种设置模式，随便吧*/
            break;
        case 33:
            adc1_config_channel_atten(ADC1_CHANNEL_5, atten);
            // adc1_config_channel_atten(ADC1_GPIO33_CHANNEL,atten);
            break;
        case 34:
            adc1_config_channel_atten(ADC1_CHANNEL_6, atten);
            // adc1_config_channel_atten(ADC1_GPIO34_CHANNEL,atten);
            break;
        case 35:
            adc1_config_channel_atten(ADC1_CHANNEL_7, atten);
            // adc1_config_channel_atten(ADC1_GPIO35_CHANNEL,atten);
            break;
        case 36:
            adc1_config_channel_atten(ADC1_CHANNEL_0, atten);
            // adc1_config_channel_atten(ADC1_GPIO36_CHANNEL,atten);
            break;
        case 37:
            adc1_config_channel_atten(ADC1_CHANNEL_1, atten);
            // adc1_config_channel_atten(ADC1_GPIO37_CHANNEL,atten);
            break;
        case 38:
            adc1_config_channel_atten(ADC1_CHANNEL_2, atten);
            // adc1_config_channel_atten(ADC1_GPIO38_CHANNEL,atten);
            break;
        case 39:
            adc1_config_channel_atten(ADC1_CHANNEL_3, atten);
            // adc1_config_channel_atten(ADC1_GPIO39_CHANNEL,atten);
            break;
        default:
            break;
        }
    }
    else
    {
        return -1;
    }
    #endif
    return 0;
}

int luat_adc_read(int pin, int *val, int *val2)
{
    #if CONFIG_IDF_TARGET_ESP32
    if (pin >= 32 && pin <= 39)
    {
        switch (pin)
        {
        case 32:
            *val = adc1_get_raw(ADC1_CHANNEL_4);
            // *val = adc1_get_raw(ADC1_GPIO32_CHANNEL); /*idf两种设置模式，随便吧*/
            *val2 = l_adc1_get_voltage(ADC1_CHANNEL_4);
            break;
        case 33:
            *val = adc1_get_raw(ADC1_CHANNEL_5);
            // *val = adc1_get_raw(ADC1_GPIO33_CHANNEL);
            *val2 = l_adc1_get_voltage(ADC1_CHANNEL_5);
            break;
        case 34:
            *val = adc1_get_raw(ADC1_CHANNEL_6);
            // *val = adc1_get_raw(ADC1_GPIO34_CHANNEL);
            *val2 = l_adc1_get_voltage(ADC1_CHANNEL_6);
            break;
        case 35:
            *val = adc1_get_raw(ADC1_CHANNEL_7);
            // *val = adc1_get_raw(ADC1_GPIO35_CHANNEL);
            *val2 = l_adc1_get_voltage(ADC1_CHANNEL_7);
            break;
        case 36:
            *val = adc1_get_raw(ADC1_CHANNEL_0);
            // *val = adc1_get_raw(ADC1_GPIO36_CHANNEL);
            *val2 = l_adc1_get_voltage(ADC1_CHANNEL_0);
            break;
        case 37:
            *val = adc1_get_raw(ADC1_CHANNEL_1);
            // *val = adc1_get_raw(ADC1_GPIO37_CHANNEL);
            *val2 = l_adc1_get_voltage(ADC1_CHANNEL_1);
            break;
        case 38:
            *val = adc1_get_raw(ADC1_CHANNEL_2);
            // *val = adc1_get_raw(ADC1_GPIO38_CHANNEL);
            *val2 = l_adc1_get_voltage(ADC1_CHANNEL_2);
            break;
        case 39:
            *val = adc1_get_raw(ADC1_CHANNEL_3);
            // *val = adc1_get_raw(ADC1_GPIO39_CHANNEL);
            *val2 = l_adc1_get_voltage(ADC1_CHANNEL_3);
            break;
        default:
            break;
        }
    }
    else
    {
        return -1;
    }
    #endif
    return 0;
}

int luat_adc_close(int pin)
{
    return 0;
}