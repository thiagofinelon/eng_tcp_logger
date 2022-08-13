#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

extern QueueHandle_t adc_read_q;
extern QueueHandle_t led_cmd_q;

enum{
    TURN_ON = 0,
    TURN_OFF,
    TOGGLE,
    LOOP_BACK,
    ADC_READ
}COMMANDS;

typedef struct
{
    uint16_t reading
} adc_message_t;

typedef struct
{
    uint8_t cmd;
    uint8_t args[128];
} command_t;

void queue_init();

#endif