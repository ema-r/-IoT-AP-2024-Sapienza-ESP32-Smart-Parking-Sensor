#pragma once

#include <stdio.h>
#include <stdint.h>
#include "esp_sleep.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/rtc_io.h"

void setup_sleeping_src();
uint64_t get_triggered_wakeup_pin();
