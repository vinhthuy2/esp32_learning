#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <driver/gpio.h>
#include "esp_log.h"

#define SHCP GPIO_NUM_16 // clock
#define STCP GPIO_NUM_17 // latch
#define DS GPIO_NUM_5 // data
#define BTN GPIO_NUM_4

static const char *TAG = "shift_register";

// array of 7-segment display digits in pull-down mode
int digits[10][8] = {
    {0, 1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 0, 1, 1, 0, 0, 0, 0}, // 1
    {0, 1, 1, 0, 1, 1, 0, 1}, // 2
    {0, 1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 0, 1, 1, 0, 0, 1, 1}, // 4
    {0, 1, 0, 1, 1, 0, 1, 1}, // 5
    {0, 1, 0, 1, 1, 1, 1, 1}, // 6
    {0, 1, 1, 1, 0, 0, 0, 0}, // 7
    {0, 1, 1, 1, 1, 1, 1, 1}, // 8
    {0, 1, 1, 1, 1, 0, 1, 1}  // 9
};

static uint8_t count = 0;

void setSegment() 
{
    gpio_set_level(STCP, 0);
    for (int i = 7; i >= 0 ; i--) {
        gpio_set_level(SHCP, 0);
        gpio_set_level(DS, !digits[count][i]);
        gpio_set_level(SHCP, 1);
    }
    gpio_set_level(STCP, 1);
}

void task1(void *pvParameter)
{
    ++count;

    if(count > 9) {
      count = 0;
    }

    ESP_LOGI(TAG, "count %d/n", count);

    setSegment();

    vTaskDelete(NULL);
}

void setup() {
    // gpio_reset_pin(STCP);
    // gpio_reset_pin(SHCP);
    // gpio_reset_pin(DS);
    // gpio_reset_pin(BTN);

    gpio_set_direction(SHCP, GPIO_MODE_OUTPUT);
    gpio_set_direction(STCP, GPIO_MODE_OUTPUT);
    gpio_set_direction(DS, GPIO_MODE_OUTPUT);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);    
    gpio_set_pull_mode(BTN, GPIO_PULLUP_ONLY);
}

extern "C" void app_main(void)
{
    setup();

    setSegment();

    while (1)
    {
        if(!gpio_get_level(BTN)) {
            ESP_LOGI(TAG, "button press");
            xTaskCreate(&task1, "task1", 2048, NULL, 1, NULL);
            // soft bouncing bypass
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }

        // to avoi empty loop
        vTaskDelay(20/portTICK_PERIOD_MS);
    }
}