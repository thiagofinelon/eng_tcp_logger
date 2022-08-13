#ifndef ADC_POLLING_H
#define ADC_POLLING_H

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100      
#define NO_OF_SAMPLES   64     

typedef struct
{
    float reading
} adc_message_t;

void adc_polling(void *args);

#endif
