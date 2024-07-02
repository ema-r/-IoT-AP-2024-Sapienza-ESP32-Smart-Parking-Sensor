#pragma once
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include <stdint.h>
#include <esp_log.h>

// Remember, nvs must be initialized beforehand:wq:wq.

void init_nvs();

void write_u16_to_nvs(const char * v_name, uint16_t v);
void read_u16_from_nvs(const char * v_name, uint16_t * v_buffer);

void write_u32_to_nvs(const char * v_name, uint32_t v);
void read_u32_from_nvs(const char * v_name, uint32_t * v_buffer);
