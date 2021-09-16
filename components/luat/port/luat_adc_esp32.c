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

uint32_t l_adc2_get_voltage(adc2_channel_t channel)
{
    static esp_adc_cal_characteristics_t *adc_chars;
    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    uint32_t adc_reading = 0;
    int temp;
    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++)
    {
        adc2_get_raw(channel, width, &temp);
        adc_reading += temp;
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
#if CONFIG_IDF_TARGET_ESP32
    case 4:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 11:
#endif
        adc2_config_channel_atten(ADC2_CHANNEL_0, atten);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 0:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 12:
#endif
        adc2_config_channel_atten(ADC2_CHANNEL_1, atten);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 2:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 13:
#endif
        adc2_config_channel_atten(ADC2_CHANNEL_2, atten);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 15:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 14:
#endif
        adc2_config_channel_atten(ADC2_CHANNEL_3, atten);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 13:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 15:
#endif
        adc2_config_channel_atten(ADC2_CHANNEL_4, atten);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 12:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 16:
#endif
        adc2_config_channel_atten(ADC2_CHANNEL_5, atten);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 14:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 17:
#endif
        adc2_config_channel_atten(ADC2_CHANNEL_6, atten);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 27:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 18:
#endif
        adc2_config_channel_atten(ADC2_CHANNEL_7, atten);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 25:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 19:
#endif
        adc2_config_channel_atten(ADC2_CHANNEL_8, atten);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 26:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 20:
#endif
        adc2_config_channel_atten(ADC2_CHANNEL_9, atten);
        break;

    default:
        return -1;
        break;
    }
#endif
    return 0;
}

int luat_adc_read(int pin, int *val, int *val2)
{
#if CONFIG_IDF_TARGET_ESP32
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

#if CONFIG_IDF_TARGET_ESP32
    case 4:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 11:
#endif
        adc2_get_raw(ADC2_CHANNEL_0, width, val);
        *val2 = l_adc2_get_voltage(ADC2_CHANNEL_0);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 0:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 12:
#endif
        adc2_get_raw(ADC2_CHANNEL_1, width, val);
        *val2 = l_adc2_get_voltage(ADC2_CHANNEL_1);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 2:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 13:
#endif
        adc2_get_raw(ADC2_CHANNEL_2, width, val);
        *val2 = l_adc2_get_voltage(ADC2_CHANNEL_2);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 15:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 14:
#endif
        adc2_get_raw(ADC2_CHANNEL_3, width, val);
        *val2 = l_adc2_get_voltage(ADC2_CHANNEL_3);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 13:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 15:
#endif
        adc2_get_raw(ADC2_CHANNEL_4, width, val);
        *val2 = l_adc2_get_voltage(ADC2_CHANNEL_4);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 12:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 16:
#endif
        adc2_get_raw(ADC2_CHANNEL_5, width, val);
        *val2 = l_adc2_get_voltage(ADC2_CHANNEL_5);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 14:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 17:
#endif
        adc2_get_raw(ADC2_CHANNEL_6, width, val);
        *val2 = l_adc2_get_voltage(ADC2_CHANNEL_6);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 27:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 18:
#endif
        adc2_get_raw(ADC2_CHANNEL_7, width, val);
        *val2 = l_adc2_get_voltage(ADC2_CHANNEL_7);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 25:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 19:
#endif
        adc2_get_raw(ADC2_CHANNEL_8, width, val);
        *val2 = l_adc2_get_voltage(ADC2_CHANNEL_8);
        break;
#if CONFIG_IDF_TARGET_ESP32
    case 26:
#elif CONFIG_IDF_TARGET_ESP32S2
    case 20:
#endif
        adc2_get_raw(ADC2_CHANNEL_9, width, val);
        *val2 = l_adc2_get_voltage(ADC2_CHANNEL_9);
        break;

    default:
        return -1;
        break;
    }
#endif
    return 0;
}

int luat_adc_close(int pin)
{
    return 0;
}
