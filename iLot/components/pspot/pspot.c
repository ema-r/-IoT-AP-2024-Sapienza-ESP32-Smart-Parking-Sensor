#include "pspot.h"
#include "soc/gpio_num.h"
#include <math.h>

static const char * TAG = "PSPOT_WAKEUP_HANDLER";

void setup_sleeping_src() {

  // Prepare the bitmask needed for ext1 wakeup enabling. Will be concat'd as input to enabling func.
  // For some reason, GPIO_NUM_1 here means gpio4.
  ESP_LOGI(TAG, "Enabling EXT1 wakeup on pins %d %d", GPIO_WUP_1, GPIO_WUP_2);

  // bit sum the bit masks with bitwise OR operator
  ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io((1ULL << GPIO_WUP_1) | (1ULL << GPIO_WUP_2), ESP_EXT1_WAKEUP_ANY_HIGH));
  ESP_ERROR_CHECK(rtc_gpio_pulldown_en(GPIO_WUP_1));
  ESP_ERROR_CHECK(rtc_gpio_pulldown_en(GPIO_WUP_2));
}


int get_triggered_wakeup_pin() {

  // Get wakeup reason
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1) {
    int wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();

    int pin_num = log(wakeup_pin_mask)/log(2);

    if(pin_num == GPIO_WUP_1){

      ESP_LOGI(TAG, "Wake up caused because of GPIO %d : ", GPIO_WUP_1);
      return 0;
    } else if(pin_num == GPIO_WUP_2){

      ESP_LOGI(TAG, "Wake up caused because of GPIO %d : ", GPIO_WUP_2);
      return 1;
    } else{
      
      ESP_LOGI(TAG, "Wake up caused because of a not setted GPIO : ");
      return -1;
    }
  } else{

    ESP_LOGI(TAG, "wakeup cause not recognized");
    return -1;
  }


}
