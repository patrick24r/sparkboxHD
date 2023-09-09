#pragma once

#include <bitset>

namespace sparkbox::controller {

enum Buttons : int {
  kButtonA = 0,
  kButtonB,
  kButtonX,
  kButtonY,
  kButtonUp,
  kButtonDown,
  kButtonLeft,
  kButtonRight,
  kButtonStart,
  kButtonSelect,

  kNumButtons,
};

class ControllerState {
 public:
  ControllerState() : connected_(false), buttons_(0) {}
  ControllerState(bool connected, uint16_t buttons) : connected_(connected), buttons_(buttons) {}
  bool Connected(void) { return connected_;  }

  bool ButtonA(void) { return buttons_[kButtonA]; }
  bool ButtonB(void) { return buttons_[kButtonB]; }
  bool ButtonX(void) { return buttons_[kButtonX]; }
  bool ButtonY(void) { return buttons_[kButtonY]; }

  bool ButtonLeft(void) { return buttons_[kButtonLeft]; }
  bool ButtonRight(void) { return buttons_[kButtonRight]; }
  bool ButtonUp(void) { return buttons_[kButtonUp]; }
  bool ButtonDown(void) { return buttons_[kButtonDown]; }

  bool ButtonStart(void) { return buttons_[kButtonStart]; }
  bool ButtonSelect(void) { return buttons_[kButtonSelect]; }
 private:
  const bool connected_;
  const std::bitset<Buttons::kNumButtons> buttons_;
};

} // namespace sparkbox::controller