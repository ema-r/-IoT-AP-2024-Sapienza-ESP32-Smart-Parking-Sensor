#pragma once

#include <stdio.h>
#include <stdint.h>
#include "esp_sleep.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/rtc_io.h"



#define GPIO_WUP_1 CONFIG_PIN1_WAKEUP
#define GPIO_WUP_2 CONFIG_PIN2_WAKEUP


void setup_sleeping_src();
int get_triggered_wakeup_pin();
