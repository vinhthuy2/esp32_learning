#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define CLK GPIO_NUM_16
#define LOAD GPIO_NUM_17
#define DIN GPIO_NUM_5
#define X_UNITS 1
#define Y_UNITS 4
#define NUM_UNITS (X_UNITS * Y_UNITS)

static const uint8_t icon[8] = {0x00,0x3c,0x62,0x52,0x4a,0x46,0x3c,0x00};
// static const uint8_t smiley[8] = {0x3c,0x42,0xa5,0x81,0xa5,0x99,0x42,0x3c};

void setup()
{
    gpio_reset_pin(CLK);
    gpio_reset_pin(LOAD);
    gpio_reset_pin(DIN);

    gpio_set_direction(CLK, GPIO_MODE_OUTPUT);
    gpio_set_direction(LOAD, GPIO_MODE_OUTPUT);
    gpio_set_direction(DIN, GPIO_MODE_OUTPUT);
}

void shiftOut(u_int8_t bitOrder, u_int8_t val)
{
    gpio_set_level(CLK, 0);
    for (u_int8_t i = 0; i < 8; i++)
    {
        if (bitOrder == 1)
        {
            gpio_set_level(DIN, !!(val & (1 << i)));
        }
        else
        {
            gpio_set_level(DIN, !!(val & (1 << (7 - i))));
        }

        // gpio_set_level(DIN, !!(val & (1 << i)));

        gpio_set_level(CLK, 1);
        vTaskDelay(pdMS_TO_TICKS(1));
        gpio_set_level(CLK, 0);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void sentData(u_int8_t address, u_int8_t data) {
    shiftOut(0, address);
    shiftOut(0, data);
}

void sentAll(u_int8_t address, u_int8_t data)
{
    for (u_int8_t i = 0; i < NUM_UNITS; ++i)
    {
        sentData(address, data);
    }
}

void updateMax7219() {
  gpio_set_level(LOAD, 1);
  vTaskDelay(pdMS_TO_TICKS(1));
  gpio_set_level(LOAD, 0);
}


void configMax7219()
{
    sentAll(0x09, 0x00); // Decode Mode: None
    sentAll(0x0A, 0x03); // Brightness: 31/32
    sentAll(0x0B, 0x07); // Scan Limit: 8 digits
    sentAll(0x0C, 0x01); // Shutdown: Normal Operation
    sentAll(0x0F, 0x00); updateMax7219();// Display Test: Normal Operation
}

void app_main(void)
{
    ESP_LOGI("main", "Hello world!");

    setup();

    configMax7219();

    while(1) {
      for (uint8_t j = 0; j < 4; j++) 
      {
        for(uint8_t i = 0; i < 8; i++) 
        {
          sentData(i + 1, icon[i]);
          sentData(i + 1, icon[i]);
          sentData(i + 1, icon[i]);
          sentData(i + 1, icon[i]);
          updateMax7219();
        }

        // vTaskDelay(pdMS_TO_TICKS(500));
      }
    }
}