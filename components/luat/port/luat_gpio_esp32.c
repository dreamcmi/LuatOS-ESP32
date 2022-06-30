/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"
#include "luat_gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "soc/gpio_reg.h"

#include "driver/gpio.h"

#define GPIO_OUT_DATA	(*(volatile unsigned int*)(GPIO_OUT_REG))

static uint8_t uart_isr_sta = 0;

extern xQueueHandle gpio_evt_queue;
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

int gpio_exist(int pin)
{
    if (pin >= 0 && pin < GPIO_NUM_MAX)
        return 1;
    else
        return 0;
}

int luat_gpio_setup(luat_gpio_t *gpio)
{
    if (!gpio_exist(gpio->pin))
        return -1;

    // 先去初始化gpio(esp32c3的io18和19必须做这一步)
    gpio_reset_pin(gpio->pin);

    //设置输入输出模式
    if (gpio->mode == Luat_GPIO_OUTPUT)
    {
        gpio_set_direction(gpio->pin, GPIO_MODE_OUTPUT);
    }
    else if (gpio->mode == Luat_GPIO_INPUT)
    {
        gpio_set_direction(gpio->pin, GPIO_MODE_INPUT);
    }
    else if (gpio->mode == Luat_GPIO_IRQ)
    {
        gpio_set_direction(gpio->pin, GPIO_MODE_INPUT);
        // //设置中断
        switch (gpio->irq)
        {
        case Luat_GPIO_RISING:
            gpio_set_intr_type(gpio->pin, GPIO_INTR_POSEDGE);
            break;
        case Luat_GPIO_FALLING:
            gpio_set_intr_type(gpio->pin, GPIO_INTR_NEGEDGE);
            break;
        case Luat_GPIO_BOTH:
        default:
            gpio_set_intr_type(gpio->pin, GPIO_INTR_ANYEDGE);
            break;
        }
        if (uart_isr_sta == 0)
        {
            gpio_install_isr_service(0);
            uart_isr_sta = 1;
        }
        gpio_isr_handler_add(gpio->pin, gpio_isr_handler, (void *)gpio->pin);
    }

    //设置上下拉
    switch (gpio->pull)
    {
    case Luat_GPIO_DEFAULT:
        gpio_set_pull_mode(gpio->pin, GPIO_FLOATING);
        break;
    case Luat_GPIO_PULLUP:
        gpio_set_pull_mode(gpio->pin, GPIO_PULLUP_ONLY);
        break;
    case Luat_GPIO_PULLDOWN:
        gpio_set_pull_mode(gpio->pin, GPIO_PULLDOWN_ONLY);
        break;
    default:
        break;
    }
    return 0;
}

int luat_gpio_set(int pin, int level)
{
    if (gpio_exist(pin))
    {
        gpio_set_level(pin, level);
        return 0;
    }
    else
        return -1;
}

int luat_gpio_get(int pin)
{
    if (gpio_exist(pin))
    {
        int level = gpio_get_level(pin);
        return level;
    }
    return -1;
}

void luat_gpio_close(int pin)
{
    if (gpio_exist(pin))
    {
        gpio_reset_pin(pin);
    }
}

void IRAM_ATTR luat_gpio_pulse(int pin, uint8_t *level, uint16_t len, uint16_t delay_ns) {
#if CONFIG_IDF_TARGET_ESP32C3
    volatile uint32_t del=delay_ns;
    vPortEnterCritical();
    for(int i=0; i<len; i++){
        if(level[i/8]&(0x80>>(i%8)))
            GPIO_OUT_DATA |= (0x00000001<<pin);
        else 
            GPIO_OUT_DATA &= ~(0x00000001<<pin);
        del = delay_ns;
        while(del--);
    }
    vPortExitCritical();
#endif
}
