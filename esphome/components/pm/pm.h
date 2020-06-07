#pragma once

#include "esphome/core/component.h"

#include "esp_pm.h"

namespace esphome {
namespace pm {

class PM : public Component {
 public:
  void setup() override;
  void disable();
  void set_freq(uint16_t min_freq_mhz, uint16_t max_freq_mhz);

  void set_tickless(bool tickless);

 private:
  uint16_t min_freq = 40;
  uint16_t max_freq = 240;
  bool tickless = false;
};

class PMLock {
 public:
  void request();

  void unrequest();

 protected:
  bool initialized = false;
  bool requested = false;
  esp_pm_lock_handle_t pm_lock_;
};

extern PM *global_pm;

}  // namespace pm
}  // namespace esphome
