#ifndef ADC_POLLING_H
#define ADC_POLLING_H

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "common.h"

#define DEFAULT_VREF    1100      
#define NO_OF_SAMPLES   64     

void adc_polling(void *args);

#endif
