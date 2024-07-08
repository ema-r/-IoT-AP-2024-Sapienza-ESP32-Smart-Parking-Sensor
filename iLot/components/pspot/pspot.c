#include "pspot.h"
#include "soc/gpio_num.h"

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
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    // If ext1 like we wanted, get the triggered GPIO pin
    uint64_t gpio_triggered = esp_sleep_get_ext1_wakeup_status();
    // For whatever reason, pin 1 appears to get printed as pin 4 on returns. Since barring this the system
    // works correctly and accepts actions on pin 1 only, we simply map the "visible" pin 4 to pin 1
    gpio_triggered = (gpio_triggered == 4) ? 1 : 2;
    ESP_LOGI(TAG, "Device was waken up by GPIO%"PRIu64, gpio_triggered);
    // We return it as 0 or 1
    gpio_triggered--;
    return gpio_triggered;
  } else {
    ESP_LOGE(TAG, "Something that isn't EXT1 happened to wake up the device. Probably the first startup.");
    return 0;
  }

}
