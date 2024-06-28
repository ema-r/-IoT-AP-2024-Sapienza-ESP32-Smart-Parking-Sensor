#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "driver/rtc_io.h"
#include "lora_com.h"
#include "crypto.h"



RTC_DATA_ATTR extern const uint8_t client_pri_key_start[] asm("_binary_private_key_client_pem_start");



// Define the ADC1 channel 0 pin (verify the correct GPIO number for your setup)
#define ADC1_CHANNEL_0 ADC1_CHANNEL_0

// GPIO number for ADC1 channel 0 (based on ESP32-S3 datasheet, update as necessary)
#define GPIO_NUM_ADC1_CH0 GPIO_NUM_1 // Update GPIO number accordingly
RTC_DATA_ATTR int boot_num=0;


char* create_message(char * message){


    mbedtls_pk_context pk;

    load_ecc_private_key(&pk, client_pri_key_start, strlen((char*) client_pri_key_start));

    unsigned char* signature=(unsigned char*) malloc(MBEDTLS_ECDSA_MAX_LEN *sizeof(unsigned char));

    size_t sig_len;

    generate_signature(message, strlen(message), 
                        signature, &sig_len, 
                        &pk, client_pri_key_start);

    
    print_exadecimal(signature, MBEDTLS_ECDSA_MAX_LEN);



    /*
    int ret= verify_signature((unsigned char*) message, strlen(message), 
                        signature, strlen((char*) signature), 
                        &pk);


    printf("the result of the signature verification is %d\n", ret);*/


    char* strings[]={"A", "0", message, (char*)signature};

    char *output_buffer = NULL;
    base64stringcat(strings, 4, &output_buffer, sig_len);
    if (output_buffer == NULL) {
        printf("error in encoding to base64 + delimiter\n");
    }
    free(signature);
    return output_buffer;
}


void app_main(void)
{
    init_lora();


    char* mes="wake up number ";
    char message[25];
    snprintf(message, sizeof(message), "%s%d",  mes, boot_num);
    char * m=create_message(message);
    printf("I am sending this message: %s\n of size %d", m, strlen(m));

    send_lora_message(m);

    free(m);
    
    printf("wake up number %d\n", boot_num);
    boot_num++;

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_ADC1_CH0, 1); // Wake up on high level
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(GPIO_NUM_1));

    printf("Entering deep sleep...\n");

            // Enter deep sleep
    esp_deep_sleep_start();



}

