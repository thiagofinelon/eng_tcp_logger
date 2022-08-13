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

void queue_init();

#endif