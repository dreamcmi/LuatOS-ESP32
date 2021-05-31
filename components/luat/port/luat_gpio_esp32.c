#include "luat_base.h"
#include "luat_gpio.h"

#include "driver/gpio.h"

//中断回调
static void gpio_cb(void *ctx)
{
    int pin = (int)ctx;
    rtos_msg_t msg = {0};
    msg.handler = l_gpio_handler;
    msg.ptr = NULL;
    msg.arg1 = pin;
    msg.arg2 = luat_gpio_get(pin);
    luat_msgbus_put(&msg, 0);
}

int luat_gpio_setup(luat_gpio_t *gpio)
{
    //选择io
    gpio_pad_select_gpio(gpio->pin);

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
            gpio_set_intr_type(gpio->pin, GPIO_INTR_ANYEDGE);
            break;
        default:
            gpio_intr_disable(gpio->pin);
            break;
        }
        gpio_isr_register(gpio_cb, (void *)(gpio->pin), ESP_INTR_FLAG_LEVEL1, NULL);
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
    gpio_set_level(pin, level);
    return 0;
}

int luat_gpio_get(int pin)
{
    int level = gpio_get_level(pin);
    return level;
}

void luat_gpio_close(int pin)
{
    gpio_reset_pin(pin);
}
