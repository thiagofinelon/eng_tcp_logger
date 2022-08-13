#include "include/led_control.h"
#include "include/common.h"
#include "driver/gpio.h"

void control_led(void *args)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = LED_OUTPUT_MASK;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(LED_OUTPUT, 0);

    uint8_t current_level = 0;
    command_t event;
    while(1)
    {
        xQueueReceive(led_cmd_q, &event, portMAX_DELAY);
        switch (event.cmd)
        {
        case TURN_ON:
            current_level = 1;
            gpio_set_level(LED_OUTPUT, 1);
            break;

        case TURN_OFF:
            current_level = 0;
            gpio_set_level(LED_OUTPUT, 0);
            break;

        case TOGGLE:
            current_level = ! current_level;
            gpio_set_level(LED_OUTPUT, current_level);
            break;

        default:
            break;
        }

    }
}