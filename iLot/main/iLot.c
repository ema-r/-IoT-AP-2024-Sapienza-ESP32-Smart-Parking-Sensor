#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "driver/rtc_io.h"
#include "lora_com.h"
#include "crypto.h"
#include "pspot.h"
#include "permstorage.h"

RTC_DATA_ATTR extern const uint8_t client_pri_key_start[] asm("_binary_private_key_client_pem_start");



// Define the ADC1 channel 0 pin (verify the correct GPIO number for your setup)
#define ADC1_CHANNEL_0 ADC1_CHANNEL_0
#define NVS_BOOTCOUNT_NAME "bootcount"

// GPIO number for ADC1 channel 0 (based on ESP32-S3 datasheet, update as necessary)
#define GPIO_NUM_ADC1_CH0 GPIO_NUM_1 // Update GPIO number accordingly
RTC_DATA_ATTR uint32_t boot_num=0;


char* create_message(char * message){

    unsigned char* signature=(unsigned char*) malloc(MBEDTLS_ECDSA_MAX_LEN *sizeof(unsigned char));

    size_t sig_len;

    mbedtls_ecdsa_context res_ctx;
    mbedtls_ecp_point Q;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    init_crypto(&Q, &entropy, &ctr_drbg);

    mbedtls_pk_context pk;

    load_ecc_private_key(&pk, (unsigned char*) client_pri_key_start, entropy, ctr_drbg);


    generate_signature(message, strlen(message), 
                        signature, &sig_len, 
                        &pk, &res_ctx, Q, entropy, ctr_drbg);


    
    print_exadecimal(signature, MBEDTLS_ECDSA_MAX_LEN);

    
    int ret= verify_signature((unsigned char*) message, strlen(message), 
                        signature, &sig_len, 
                        &pk, res_ctx, Q, entropy, ctr_drbg);


    free_crypto(Q, entropy, ctr_drbg);
    printf("the result of the signature verification is %d\n", ret);


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
    init_nvs();
    setup_sleeping_src();

    if (boot_num == 0) {
      // Check if there's a boot count written on the NVS
      uint32_t nvs_bootcount_buffer = 0;
      read_u32_from_nvs(NVS_BOOTCOUNT_NAME, &nvs_bootcount_buffer);
      printf("wake up number on nvs: %ld\n", nvs_bootcount_buffer);
      if (nvs_bootcount_buffer != 0) {
        boot_num = nvs_bootcount_buffer;
      } // Add some else code that generates a random starting number
    }

    printf("wake up number %ld\n", boot_num);

    // Get wakeup num, create message by concat'ing boot_num:wakeup_pin
    uint64_t wakeup_pin = get_triggered_wakeup_pin();
    char message[80];
    snprintf(message, sizeof(message), "%ld:%lld",  boot_num, wakeup_pin);
    printf("\nWRITTEN MESSAGE IS %s\n", message);
    char * m=create_message(message);
    printf("I am sending this message: %s\n of size %d", m, strlen(m));

    //send_lora_message(m);

    free(m);
    
    boot_num++;
    // Write the updated boot count to nvs memory in case power is completely lost.
    write_u32_to_nvs(NVS_BOOTCOUNT_NAME, boot_num);

    printf("Entering deep sleep...\n");

    //        // Enter deep sleep
    esp_deep_sleep_start();



}
