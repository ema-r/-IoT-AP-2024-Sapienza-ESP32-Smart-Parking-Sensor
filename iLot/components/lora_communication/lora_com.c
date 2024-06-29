#include "lora_com.h"

#include <stdio.h>
#include "driver/uart.h"
#include "esp_log.h"


static const char *TAG = "LORA";
uint32_t frequencyInHz = 0;
#define TIMEOUT 100

uint8_t txData[256]; // Maximum Payload size of SX1261/62/68 is 255
uint8_t rxData[256]; // Maximum Payload size of SX1261/62/68 is 255


void init_lora() {

    #if CONFIG_169MHZ
        frequencyInHz = 169000000;
        ESP_LOGI(TAG, "Frequency is 169MHz");
    #elif CONFIG_433MHZ
        frequencyInHz = 433000000;
        ESP_LOGI(TAG, "Frequency is 433MHz");
    #elif CONFIG_470MHZ
        frequencyInHz = 470000000;
        ESP_LOGI(TAG, "Frequency is 470MHz");
    #elif CONFIG_866MHZ
        frequencyInHz = 866000000;
        ESP_LOGI(TAG, "Frequency is 866MHz");
    #elif CONFIG_915MHZ
        frequencyInHz = 915000000;
        ESP_LOGI(TAG, "Frequency is 915MHz");
    #elif CONFIG_OTHER
        ESP_LOGI(TAG, "Frequency is %dMHz", CONFIG_OTHER_FREQUENCY);
        frequencyInHz = CONFIG_OTHER_FREQUENCY * 1000000;
    #endif

        // Initialize LoRa
        LoRaInit();
        int8_t txPowerInDbm = 22;

    #if CONFIG_USE_TCXO
        ESP_LOGW(TAG, "Enable TCXO");
        float tcxoVoltage = 3.3; // use TCXO
        bool useRegulatorLDO = true; // use DCDC + LDO
    #else
        ESP_LOGW(TAG, "Disable TCXO");
        float tcxoVoltage = 0.0;  // don't use TCXO
        bool useRegulatorLDO = false;  // use only LDO in all modes
    #endif
        if (LoRaBegin(frequencyInHz, txPowerInDbm, tcxoVoltage, useRegulatorLDO) != 0) {
            ESP_LOGE(TAG, "Does not recognize the module");
            while(1) {
                vTaskDelay(1);
            }
        }
        uint8_t spreadingFactor = 7;
        uint8_t bandwidth = 4;
        uint8_t codingRate = 1;
        uint16_t preambleLength = 8;
        uint8_t payloadLen = 0;
        bool crcOn = true;
        bool invertIrq = false;
    #if CONFIF_ADVANCED
        spreadingFactor = CONFIG_SF_RATE;
        bandwidth = CONFIG_BANDWIDTH;
        codingRate = CONFIG_CODING_RATE
    #endif
        LoRaConfig(spreadingFactor, bandwidth, codingRate, preambleLength, payloadLen, crcOn, invertIrq);

}



void receive_lora_data(){

    TickType_t startTick = xTaskGetTickCount();
    while(1) {
        uint8_t rxLen = LoRaReceive(rxData, sizeof(rxData));
        TickType_t currentTick = xTaskGetTickCount();
        TickType_t diffTick = currentTick - startTick;
        if ( rxLen > 0 ) {
            ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rxLen, rxLen, rxData);
            ESP_LOGI(pcTaskGetName(NULL), "Response time is %"PRIu32" millisecond", diffTick * portTICK_PERIOD_MS);
        }
        
        vTaskDelay(TIMEOUT); // Avoid WatchDog alerts
    } // end waiting
    
}

void send_lora_message(const char* message) {

    ESP_LOGI(TAG, "Start");
    TickType_t nowTick = xTaskGetTickCount();
    int txLen = sprintf((char *)txData, message, nowTick);
    //uint8_t len = strlen((char *)txData);

    // Wait for transmission to complete
    if (LoRaSend(txData, txLen, SX126x_TXMODE_SYNC)) {
        ESP_LOGI(TAG, "Send success");
    } else {
        ESP_LOGE(TAG, "Send fail");
    }



}
