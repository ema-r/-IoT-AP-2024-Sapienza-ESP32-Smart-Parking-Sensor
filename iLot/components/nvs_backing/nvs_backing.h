#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include <stdint.h>
#include <esp_log.h>

// Remember, nvs must be initialized beforehand:wq:wq.

void write_u16_to_nvs(const char * v_name, uint16_t v);
