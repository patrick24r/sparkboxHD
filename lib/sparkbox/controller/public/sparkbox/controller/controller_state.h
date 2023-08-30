#pragma once

#include <bitset>

namespace sparkbox::controller {

class AnalogStickState {
 public:
  AnalogStickState(float x, float y) : x_(x), y_(y) {}

  float X(void) { return x_; }
  float Y(void) { return y_; }

 private:
  const float x_ = 0.0;
  const float y_ = 0.0;
};

enum Buttons : int {
  kButtonA = 0,
  kButtonB,
  kButtonX,
  kButtonY,
  kButtonL,
  kButtonR,
  kButtonStart,
  kButtonSelect,

  kNumButtons,
};

class ControllerState {
 public:
  ControllerState(size_t buttons, AnalogStickState left, AnalogStickState right)
    : buttons_(buttons), analog_left_(left), analog_right_(right) {}
  bool ButtonA(void) { return buttons_[kButtonA]; }
  bool ButtonB(void) { return buttons_[kButtonB]; }
  bool ButtonX(void) { return buttons_[kButtonX]; }
  bool ButtonY(void) { return buttons_[kButtonY]; }
  bool ButtonStart(void) { return buttons_[kButtonStart]; }
  bool ButtonSelect(void) { return buttons_[kButtonSelect]; }

  AnalogStickState& AnalogLeft(void) { return analog_left_; }
  AnalogStickState& AnalogRight(void) { return analog_left_; }
 private:
  const std::bitset<Buttons::kNumButtons> buttons_;
  AnalogStickState analog_left_;
  AnalogStickState analog_right_;
};

} // namespace sparkbox::controller