#include "nvs_backing.h"

static char * TAG = "NVS_HANDLER";

void write_u16_to_nvs(const char * v_name, uint16_t v) {
  nvs_handle_t n_handle;
  esp_err_t ret = nvs_open("storage", NVS_READONLY, &n_handle);
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "Error opening nvs storage, error: %s", esp_err_to_name(ret));
  } else {
    // Write to nvs
    ESP_LOGI(TAG, "Updating value %s in NVS storage...", v_name);

    ret = nvs_set_u16(n_handle, v_name, v);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");

    printf("Committing updates in nvs... ");
    ret = nvs_commit(n_handle);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");

    // Close
    nvs_close(n_handle);
  }
}

void read_u16_from_nvs(const char * v_name, uint16_t * v_buffer) {
  nvs_handle_t n_handle;
  esp_err_t ret = nvs_open("storage", NVS_READONLY, &n_handle);
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "Error opening nvs storage, error: %s", esp_err_to_name(ret));
  } else {

    ESP_LOGI(TAG, "Retrieving value %s in NVS storage...", v_name);

    ret = nvs_get_u16(n_handle, v_name, v_buffer);
    switch (ret) {
        case ESP_OK:
            printf("Done\n");
            printf("%s = %" PRIu16 "\n", v_name, *v_buffer);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(ret));
    }

    nvs_close(n_handle);
  }
}
