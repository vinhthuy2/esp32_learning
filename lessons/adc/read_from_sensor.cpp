#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <driver/adc.h>
#include <math.h>

#define SENSOR_IN ADC1_CHANNEL_6 //GPIO34
const float BETA = 3950; // should match the Beta Coefficient of the thermistor
const float MAX_VAL_12BIT = 4095.;

void app_main() {
  printf("Hello, Wokwi!\n");

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(SENSOR_IN,ADC_ATTEN_DB_11);

  while (true) {
    int read_raw = adc1_get_raw(SENSOR_IN);
    float celsius = 1 / (log(1 / (MAX_VAL_12BIT / read_raw - 1)) / BETA + 1.0 / 298.15) - 273.15;
    printf("Temperature: %f ℃\n", celsius);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
