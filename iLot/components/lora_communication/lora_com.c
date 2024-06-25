#include "lora_com.h"

#include <stdio.h>
#include "driver/uart.h"
#include "esp_log.h"

#define UART_NUM UART_NUM_1
#define TXD_PIN (UART_PIN_NO_CHANGE)
#define RXD_PIN (UART_PIN_NO_CHANGE)
#define BUF_SIZE (1024)



static const char *TAG = "uart_example";

void init_uart() {

}

void deinit_uart(){
}

void receive_lora_data(){
    
}

void send_lora_message(const char* message) {
    
}
