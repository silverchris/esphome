#pragma once

#include "esphome/core/component.h"
#include "esphome/core/esphal.h"
#include "esphome/core/application.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "soc/rtc.h"
#include  "freertos/task.h"


#ifdef CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

namespace esphome {
namespace pm {

static const char *TAG = "PM";

class PM : public Component {
 public:
  void setup() override{
#if CONFIG_PM_ENABLE
    ESP_LOGI(TAG, "PM Enabled");
    // Configure dynamic frequency scaling:
    // maximum and minimum frequencies are set in sdkconfig,
    // automatic light sleep is enabled if tickless idle support is enabled.
    esp_pm_config_esp32_t pm_config;
            pm_config.max_freq_mhz = max_freq;
            pm_config.min_freq_mhz = min_freq;
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
            pm_config.light_sleep_enable = true;
	    ESP_LOGI(TAG, "Tickless");
#endif
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
#endif // CONFIG_PM_ENABLE
    App.set_loop_interval(250);
  }
  void set_freq(uint16_t min_freq_mhz, uint16_t max_freq_mhz) {
    this->min_freq = min_freq_mhz;
    this->max_freq = max_freq_mhz;
  }

 private:
  uint16_t min_freq = 40;
  uint16_t max_freq = 80;
};

class PMLock {
 public:
  void request() {
    if(!this->initialized){
      esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "Output Lock", &this->pm_lock_);
      this->initialized = true;
    }
    if (!this->requested) {
      esp_pm_lock_acquire(this->pm_lock_);
      this->requested = true;
    }
  }
  void unrequest() {
    if (this->requested && this->initialized) {
      esp_pm_lock_release(this->pm_lock_);
      this->requested = false;
    }
  }

 protected:
  bool initialized = false;
  bool requested = false;
  esp_pm_lock_handle_t pm_lock_;
};

}  // namespace pm
}  // namespace esphome
