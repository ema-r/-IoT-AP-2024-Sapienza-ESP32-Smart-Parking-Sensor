#include "pspot.h"

static const char * TAG = "PSPOT_WAKEUP_HANDLER";

void setup_sleeping_src() {

  // Prepare the bitmask needed for ext1 wakeup enabling. Will be concat'd as input to enabling func.
  const int ext_wakeup_pin_1 = GPIO_NUM_1;
  const int ext_wakeup_pin_2 = GPIO_NUM_2;
  const uint64_t ext_wakeup_pin_1_mask = 1ULL << ext_wakeup_pin_1;
  const uint64_t ext_wakeup_pin_2_mask = 1ULL << ext_wakeup_pin_2;
  ESP_LOGI(TAG, "Enabling EXT1 wakeup on pins GPIO%d, GPIO%d\n", ext_wakeup_pin_1, ext_wakeup_pin_2);

  ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io(ext_wakeup_pin_1_mask | ext_wakeup_pin_2_mask, ESP_EXT1_WAKEUP_ANY_HIGH));
}

uint64_t get_triggered_wakeup_pin() {

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    uint64_t gpio_triggered = esp_sleep_get_ext1_wakeup_status();
    ESP_LOGI(TAG, "Device was waken up by GPIO%"PRIu64, gpio_triggered);
    return gpio_triggered;
  } else {
    ESP_LOGE(TAG, "Something that isn't EXT1 happened to wake up the device. Check your code for possible demonic possession.");
    return 0;
  }

}
