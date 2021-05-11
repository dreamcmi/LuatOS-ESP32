#include "luat_base.h"
#include "luat_adc.h"

#include "driver/adc.h"

int luat_adc_open(int pin, void* args)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    if(pin>=32 && pin<=39)
    {
        switch (pin)
        {
        case 32:
            adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_0);
            break;
        case 33:
            adc1_config_channel_atten(ADC1_CHANNEL_5,ADC_ATTEN_DB_0);
            break;
        case 34:
            adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_0);
            break;
        case 35:
            adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_0);
            break;
        case 36:
            adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);
            break;
        case 37:
            adc1_config_channel_atten(ADC1_CHANNEL_1,ADC_ATTEN_DB_0);
            break;
        case 38:
            adc1_config_channel_atten(ADC1_CHANNEL_2,ADC_ATTEN_DB_0);
            break;
        case 39:
            adc1_config_channel_atten(ADC1_CHANNEL_3,ADC_ATTEN_DB_0);
            break;
        default:
            break;
        }
    }

    /*
    //这个宏定义能不能拼接呢 idf给了好几种设置模式
    */
    // if(pin>=32 && pin<=39)
    // {
    //     switch (pin)
    //     {
    //     case 32:
            // adc1_config_channel_atten(ADC1_GPIO32_CHANNEL,ADC_ATTEN_DB_0);
    //         break;
    //     case 33:
    //         adc1_config_channel_atten(ADC1_GPIO33_CHANNEL,ADC_ATTEN_DB_0);
    //         break;
    //     case 34:
    //         adc1_config_channel_atten(ADC1_GPIO34_CHANNEL,ADC_ATTEN_DB_0);
    //         break;
    //     case 35:
    //         adc1_config_channel_atten(ADC1_GPIO35_CHANNEL,ADC_ATTEN_DB_0);
    //         break;
    //     case 36:
    //         adc1_config_channel_atten(ADC1_GPIO36_CHANNEL,ADC_ATTEN_DB_0);
    //         break;
    //     case 37:
    //         adc1_config_channel_atten(ADC1_GPIO37_CHANNEL,ADC_ATTEN_DB_0);
    //         break;
    //     case 38:
    //         adc1_config_channel_atten(ADC1_GPIO38_CHANNEL,ADC_ATTEN_DB_0);
    //         break;
    //     case 39:
    //         adc1_config_channel_atten(ADC1_GPIO39_CHANNEL,ADC_ATTEN_DB_0);
    //         break;
    //     default:
    //         break;
    //     }
    // }
    else
    {
        return -1;
    }
    return 0;
}

int luat_adc_read(int pin, int* val, int* val2)
{
    if(pin>=32 && pin<=39)
    {
        switch (pin)
        {
        case 32:
            *val = adc1_get_raw(ADC1_CHANNEL_4);
            // *val = adc1_get_raw(ADC1_GPIO32_CHANNEL);
            *val2 = 0; //TODO
            break;
        case 33:
            *val = adc1_get_raw(ADC1_CHANNEL_5);
            // *val = adc1_get_raw(ADC1_GPIO33_CHANNEL);
            *val2 = 0; //TODO
            break;
        case 34:
            *val = adc1_get_raw(ADC1_CHANNEL_6);
            // *val = adc1_get_raw(ADC1_GPIO34_CHANNEL);
            *val2 = 0; //TODO
            break;
        case 35:
            *val = adc1_get_raw(ADC1_CHANNEL_7);
            // *val = adc1_get_raw(ADC1_GPIO35_CHANNEL);
            *val2 = 0; //TODO
            break;
        case 36:
            *val = adc1_get_raw(ADC1_CHANNEL_0);
            // *val = adc1_get_raw(ADC1_GPIO36_CHANNEL);
            *val2 = 0; //TODO
            break;
        case 37:
            *val = adc1_get_raw(ADC1_CHANNEL_1);
            // *val = adc1_get_raw(ADC1_GPIO37_CHANNEL);
            *val2 = 0; //TODO
            break;
        case 38:
            *val = adc1_get_raw(ADC1_CHANNEL_2);
            // *val = adc1_get_raw(ADC1_GPIO38_CHANNEL);
            *val2 = 0; //TODO
            break;
        case 39:
            *val = adc1_get_raw(ADC1_CHANNEL_3);
            // *val = adc1_get_raw(ADC1_GPIO39_CHANNEL);
            *val2 = 0; //TODO
            break;
        default:
            break;
        }
    }
    else
    {
        return -1;
    }
    return 0;
}

int luat_adc_close(int pin)
{
    return 0;
}