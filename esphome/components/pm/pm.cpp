#include "esphome/core/esphal.h"
#include "esphome/core/application.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "soc/rtc.h"
#include "freertos/task.h"

#include "esp_pm.h"


#include "pm.h"

namespace esphome {
namespace pm {

static const char *TAG = "PM";

void PM::setup() {
#if CONFIG_PM_ENABLE
  ESP_LOGI(TAG, "PM Support Enabled");
  // Configure dynamic frequency scaling:
  // maximum and minimum frequencies are set in sdkconfig,
  // automatic light sleep is enabled if tickless idle support is enabled.
  esp_pm_config_esp32_t pm_config;
  pm_config.max_freq_mhz = max_freq;
  pm_config.min_freq_mhz = min_freq;
  ESP_LOGI(TAG, "Setting Minimum frequency to %dMHz, Maximum to %dMHz", min_freq, max_freq);
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
  pm_config.light_sleep_enable = this->tickless;
  if (this->tickless) {
    ESP_LOGI(TAG, "Tickless Idle Enabled");
  }
  else {
      ESP_LOGI(TAG, "Tickless Idle Disabled");
  }
#else
  ESP_LOGI(TAG, "Tickless Idle Support Disabled");
#endif
  ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
#else
  ESP_LOGI(TAG, "PM Support Disabled");
#endif  // CONFIG_PM_ENABLE
  App.set_loop_interval(250);
}

void PM::set_freq(uint16_t min_freq_mhz, uint16_t max_freq_mhz) {
  this->min_freq = min_freq_mhz;
  this->max_freq = max_freq_mhz;
}

void PM::set_tickless(bool tickless) { this->tickless = tickless; }

void PMLock::request() {
#if CONFIG_PM_ENABLE
  if (!this->initialized) {
    esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "Output Lock", &this->pm_lock_);
    this->initialized = true;
  }
  if (!this->requested) {
    esp_pm_lock_acquire(this->pm_lock_);
    this->requested = true;
  }
#endif
}

void PMLock::unrequest() {
#if CONFIG_PM_ENABLE
  if (this->requested && this->initialized) {
    esp_pm_lock_release(this->pm_lock_);
    this->requested = false;
  }
#endif
}
}  // namespace pm
}  // namespace esphome