#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#include "esp32/rom/ets_sys.h"
#include <esp32-hal.h>

#include "easyscale.h"

namespace esphome {
namespace easyscale {

static const char *TAG = "EasyScale";

void EasyScale::setup() {
  this->pin_->setup();

  noInterrupts();
  // reset the chip, by holding the control line low.
  this->pin_->digital_write(LOW);
  delay(1000);

  this->pin_->digital_write(HIGH);
  ets_delay_us(200);
  this->pin_->digital_write(LOW);
  ets_delay_us(500);
  this->pin_->digital_write(HIGH);
  interrupts();

#ifdef USE_PM
  gpio_hold_en((gpio_num_t) this->pin_->get_pin());
#endif
}

void EasyScale::sendlow() {
  this->pin_->digital_write(LOW);
  ets_delay_us(80);
  this->pin_->digital_write(HIGH);
  ets_delay_us(20);
}

void EasyScale::sendhigh() {
  this->pin_->digital_write(LOW);
  ets_delay_us(20);
  this->pin_->digital_write(HIGH);
  ets_delay_us(80);
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

  uint8_t out = (31 / 1 * state) + 0.5;
  if (out != this->last_state) {
#ifdef USE_PM
    gpio_hold_dis((gpio_num_t) this->pin_->get_pin());
#endif
    noInterrupts();
    for (int i = 7; i >= 0; i--) {
      if ((this->device_address_ >> i) & 1) {
        this->sendhigh();
      } else {
        this->sendlow();
      }
    }
    this->pin_->digital_write(LOW);
    ets_delay_us(10);
    this->pin_->digital_write(HIGH);
    ets_delay_us(10);

    this->sendlow();
    this->sendlow();
    this->sendlow();

    for (int i = 4; i >= 0; i--) {
      if ((out >> i) & 1) {
        this->sendhigh();
      } else {
        this->sendlow();
      }
    }
    this->last_state = out;
    this->pin_->digital_write(LOW);
    ets_delay_us(100);
    this->pin_->digital_write(HIGH);
    ets_delay_us(10);
    interrupts();
#ifdef USE_PM
    gpio_hold_en((gpio_num_t) this->pin_->get_pin());
#endif
  }
}

}  // namespace easyscale
}  // namespace esphome
