#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <driver/gpio.h>

#define SRCLK GPIO_NUM_6 // clock
#define RCLK GPIO_NUM_7 // latch
#define DS GPIO_NUM_8 // data
#define BTN GPIO_NUM_2

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

static uint8_t count = -1;

void task1(void *pvParameter)
{
    ++count;

    gpio_set_level(RCLK, 0);
    for (int i = 7; i >= 0 ; i--) {
        gpio_set_level(DS, digits[count][i]);
        gpio_set_level(SRCLK, 1);
        gpio_set_level(SRCLK, 0);
    }
    gpio_set_level(RCLK, 1);
    vTaskDelete(NULL);
}

void setup() {
    gpio_reset_pin(SRCLK);
    gpio_reset_pin(RCLK);
    gpio_reset_pin(DS);
    
    gpio_set_direction(SRCLK, GPIO_MODE_OUTPUT);
    gpio_set_direction(RCLK, GPIO_MODE_OUTPUT);
    gpio_set_direction(DS, GPIO_MODE_OUTPUT);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);    
    gpio_set_pull_mode(BTN, GPIO_PULLUP_ONLY);
}

extern "C" void app_main(void)
{
    setup();

    while (1)
    {
        if(!gpio_get_level(BTN)) {
            xTaskCreate(&task1, "task1", 2048, NULL, 5, NULL);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
    }
}