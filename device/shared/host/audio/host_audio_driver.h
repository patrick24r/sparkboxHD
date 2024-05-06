#pragma once

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/status.h"

namespace device::shared::host {

class HostAudioDriver : public sparkbox::audio::AudioDriver {
 public:
  sparkbox::Status SetUp(void) final { return sparkbox::Status::kOk; }
  sparkbox::Status TearDown(void) final { return sparkbox::Status::kOk; }
};

}  // namespace device::shared::host