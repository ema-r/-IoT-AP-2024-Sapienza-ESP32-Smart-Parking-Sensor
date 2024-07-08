#include "permstorage.h"

static char * TAG = "NVS_HANDLER";

// Init nvs, check for errors
void init_nvs() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
  
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}

// Get an handle, attempt opening the nvs storage with the right flag, we set the value then commit
void write_u16_to_nvs(const char * v_name, uint16_t v) {
  nvs_handle_t n_handle;
  esp_err_t ret = nvs_open("storage", NVS_READWRITE, &n_handle);
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

// Like write op, but since we only read there's no need to commit modifications
// to the nvs.
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

void write_u32_to_nvs(const char * v_name, uint32_t v) {
  nvs_handle_t n_handle;
  esp_err_t ret = nvs_open("storage", NVS_READWRITE, &n_handle);
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "Error opening nvs storage, error: %s", esp_err_to_name(ret));
  } else {
    // Write to nvs
    ESP_LOGI(TAG, "Updating value %s in NVS storage...", v_name);

    ret = nvs_set_u32(n_handle, v_name, v);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");

    printf("Committing updates in nvs... ");
    ret = nvs_commit(n_handle);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");

    // Close
    nvs_close(n_handle);
  }
}

void read_u32_from_nvs(const char * v_name, uint32_t * v_buffer) {
  nvs_handle_t n_handle;
  esp_err_t ret = nvs_open("storage", NVS_READONLY, &n_handle);
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "Error opening nvs storage, error: %s", esp_err_to_name(ret));
  } else {

    ESP_LOGI(TAG, "Retrieving value %s in NVS storage...", v_name);

    ret = nvs_get_u32(n_handle, v_name, v_buffer);
    switch (ret) {
        case ESP_OK:
            printf("Done\n");
            printf("%s = %" PRIu32 "\n", v_name, *v_buffer);
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
