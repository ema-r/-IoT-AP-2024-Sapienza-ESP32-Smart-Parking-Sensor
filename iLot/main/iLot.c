#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "driver/rtc_io.h"
#include "lora_com.h"



// Define the ADC1 channel 0 pin (verify the correct GPIO number for your setup)
#define ADC1_CHANNEL_0 ADC1_CHANNEL_0

// GPIO number for ADC1 channel 0 (based on ESP32-S3 datasheet, update as necessary)
#define GPIO_NUM_ADC1_CH0 GPIO_NUM_1 // Update GPIO number accordingly
RTC_DATA_ATTR int boot_num=0;

void app_main(void)
{
    /*printf("wake up number %d\n", boot_num);
    boot_num++;

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_ADC1_CH0, 1); // Wake up on high level
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(GPIO_NUM_1));

    printf("Entering deep sleep...\n");

            // Enter deep sleep
    esp_deep_sleep_start();*/



init_lora();


send_lora_message("ciao bello");





}

