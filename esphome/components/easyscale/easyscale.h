#pragma once

#include "esphome/components/output/float_output.h"


namespace esphome {
namespace easyscale {

class EasyScale : public output::FloatOutput, public Component {
 public:
  explicit EasyScale(GPIOPin *pin) : pin_(pin) { }
  void setup() override;

  /// Override FloatOutput's write_state.
  void write_state(float state) override;

  void turn_on() override {}
  void turn_off() override {}

 protected:
  GPIOPin *pin_;
  float duty_{0.0f};
  uint8_t device_address_ = 72;
};

}  // namespace easyscale
}  // namespace esphome
