#include "include/adc_polling.h"
#include "esp_log.h"

esp_adc_cal_characteristics_t *adc_chars;
const adc_channel_t channel = ADC_CHANNEL_6;
const adc_bits_width_t width = ADC_WIDTH_BIT_12;
const adc_atten_t atten = ADC_ATTEN_DB_11;
const adc_unit_t unit = ADC_UNIT_1;

/*
 * @brief: Essa task verifica o adc e enviará esse dado para outras task atráves de uma fila
 *
 */
void adc_polling(void *args)
{
    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);

    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);

    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);

    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);

    uint32_t voltage = 0;
    uint32_t adc_reading = 0;
    adc_message_t adc;
    while (1)
    {
        adc_reading = 0;
        for (int i = 0; i < NO_OF_SAMPLES; i++)
        {
    
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        }
        adc_reading /= NO_OF_SAMPLES;
        voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        adc.reading = voltage;

        xQueueSend(adc_read_q, &adc, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}