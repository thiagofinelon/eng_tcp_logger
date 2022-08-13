#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define LED_OUTPUT  2

typedef struct
{
    uint8_t cmd
} led_command_t;

enum{
    TURN_ON = 0,
    TURN_OFF,
    TOGGLE
}LED_COMMANDS;

void led_io_control(void *args);


#endif
