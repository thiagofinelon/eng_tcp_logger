#include <stdio.h>
#include "include/common.h"
#include "include/led_control.h"
#include "include/adc_polling.h"
#include "include/wifi.h"
#include "include/tcp_server.h"

void app_main(void)
{
    wifi_init_softap();
    queue_init();

    xTaskCreatePinnedToCore(control_led, "ctrl_led", 8192, NULL, 5 , NULL, 1);
    xTaskCreatePinnedToCore(adc_polling, "adc_read", 8192, NULL, 5 , NULL, 1);
    xTaskCreatePinnedToCore(tcp_server, "tcp_server", 4096, NULL, 5, NULL, 1);
   

    while (1)
    {   
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
