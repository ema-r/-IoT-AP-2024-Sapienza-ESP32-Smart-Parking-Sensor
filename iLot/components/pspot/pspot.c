#include "pspot.h"
#include "soc/gpio_num.h"
#include <math.h>

static const char * TAG = "PSPOT_WAKEUP_HANDLER";

void setup_sleeping_src() {

  // Prepare the bitmask needed for ext1 wakeup enabling. Will be concat'd as input to enabling func.
  // For some reason, GPIO_NUM_1 here means gpio4.
  const int ext_wakeup_pin_1 = GPIO_WUP_1;
  const int ext_wakeup_pin_2 = GPIO_WUP_2;
  const uint64_t ext_wakeup_pin_1_mask = 1ULL << ext_wakeup_pin_1;  // turns 1 into unsigned long long. Shifts bit by pin value, moving the 1
  const uint64_t ext_wakeup_pin_2_mask = 1ULL << ext_wakeup_pin_2;
  ESP_LOGI(TAG, "Enabling EXT1 wakeup on pins GPIO1, GPIO2");

  // bit sum the bit masks with bitwise OR operator
  ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io(ext_wakeup_pin_1_mask | ext_wakeup_pin_2_mask, ESP_EXT1_WAKEUP_ANY_HIGH));
  ESP_ERROR_CHECK(rtc_gpio_pulldown_en(GPIO_WUP_1));
  ESP_ERROR_CHECK(rtc_gpio_pulldown_en(GPIO_WUP_2));
}

uint64_t get_triggered_wakeup_pin() {

  // Get wakeup reason
  //esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  uint64_t GPIO_number = esp_sleep_get_ext1_wakeup_status();
  int actual_pin = (int) ((log(GPIO_number))/log(2)); 
  ESP_LOGI(TAG, "Wake up caused because of GPIO: ");
  ESP_LOGI(TAG, "%d", actual_pin);

  if (actual_pin == GPIO_WUP_1) {
    return 1;
  } else if (actual_pin == GPIO_WUP_2) {
    return 2;
  } else {
    return 0;
  }
}
