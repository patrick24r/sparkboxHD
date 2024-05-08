#pragma once

#include <cstdint>
#include <functional>
#include <span>

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/status.h"

namespace device::shared::host {

class HostAudioDriver : public sparkbox::audio::AudioDriver {
 public:
  sparkbox::Status SetUp(void) final { return sparkbox::Status::kOk; }
  sparkbox::Status TearDown(void) final { return sparkbox::Status::kOk; }

  sparkbox::Status PlaybackStart(void) final { return sparkbox::Status::kOk; }

  sparkbox::Status WriteSampleBlock(std::span<int16_t> samples, bool is_mono,
                                    uint32_t sample_rate) final {
    return sparkbox::Status::kOk;
  }

  sparkbox::Status PlaybackStop(void) final { return sparkbox::Status::kOk; }

  sparkbox::Status SetOnSampleBlockComplete(Callback& callback) final;

 private:
  Callback on_sample_complete_cb_ = NULL;
};

}  // namespace device::shared::host