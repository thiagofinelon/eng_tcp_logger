#include <stdio.h>
#include "include/adc_polling.h"
#include "include/led_control.h"
#include "include/common.h"

void app_main(void)
{
    queue_init();

    xTaskCreate(adc_polling, "adc_read", 4096, NULL, 8, NULL);
    xTaskCreate(led_io_control, "led_contrl", 4096, NULL, 8, NULL);

    while (1)
    {
        vTaskDelay(1);
    }
}
