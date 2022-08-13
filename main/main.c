#include <stdio.h>
#include "include/adc_polling.h"

void app_main(void)
{
    xTaskCreate(adc_polling, "adc_read", 4096, NULL, 2, NULL);

    while (1)
    {
        vTaskDelay(1);
    }
}
