#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#include "rom/ets_sys.h"
#include <esp32-hal.h>

#include "easyscale.h"

namespace esphome {
namespace easyscale {

static const char *TAG = "EasyScale";

void EasyScale::setup() {
  this->pin_->setup();
  this->pin_->digital_write(true);
  ets_delay_us(100);
  this->pin_->digital_write(false);
  ets_delay_us(260);
  this->pin_->digital_write(true);

#ifdef USE_PM
  gpio_hold_en((gpio_num_t) this->pin_->get_pin());
#endif
}

void EasyScale::write_state(float state) {
  state = clamp(state, 0.0f, 1.0f);

#ifdef USE_POWER_SUPPLY
  if (state > 0.0f) {  // ON
    this->power_.request();
  } else {  // OFF
    this->power_.unrequest();
  }
#endif

  float adjusted_value = (state * (this->max_power_ - this->min_power_)) + this->min_power_;
  if (this->is_inverted())
    adjusted_value = 1.0f - adjusted_value;

#ifdef USE_PM
  gpio_hold_dis((gpio_num_t) this->pin_->get_pin());
#endif
  for (uint8_t i = 0; i < 8; i++) {
    this->pin_->digital_write((this->device_address_ >> i) & 1);
  }
  uint8_t out = (31 / 1 * state) + 0.5;
  for (int i = 4; i >= 0; i--) {
    this->pin_->digital_write((out >> i) & 1);
  }

  this->pin_->digital_write(false);
  ets_delay_us(100);
  this->pin_->digital_write(true);
  ets_delay_us(10);


#ifdef USE_PM
  gpio_hold_en((gpio_num_t) this->pin_->get_pin());
#endif
}

}  // namespace easyscale
}  // namespace esphome
