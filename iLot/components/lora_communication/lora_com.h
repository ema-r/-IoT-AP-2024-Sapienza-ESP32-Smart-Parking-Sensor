#include "string.h"
#include "driver/uart.h"
#include "sdkconfig.h"
#include "stdio.h"
#include "esp_log.h"

void init_lora();
void deinit_uart();
void send_lora_message(const char* message);
void receive_lora_data();
