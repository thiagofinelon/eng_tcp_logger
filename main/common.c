#include "include/common.h"
#include "include/adc_polling.h"
#include "include/led_control.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

QueueHandle_t adc_read_q;
QueueHandle_t led_cmd_q;

void queue_init()
{
    adc_read_q = xQueueCreate(1, sizeof(adc_message_t));
    led_cmd_q = xQueueCreate(1, sizeof(command_t));
}
