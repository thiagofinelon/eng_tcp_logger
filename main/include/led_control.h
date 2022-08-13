#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define LED_OUTPUT GPIO_NUM_2
#define LED_OUTPUT_MASK (1ULL << LED_OUTPUT)

void control_led(void *args);


#endif
