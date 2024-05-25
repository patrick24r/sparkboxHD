#pragma once

#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <span>

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/status.h"

namespace device::shared::host {

class HostAudioDriver : public sparkbox::audio::AudioDriver {
 public:
  sparkbox::Status SetUp(void) final { return sparkbox::Status::kOk; }
  sparkbox::Status TearDown(void) final { return sparkbox::Status::kOk; }

  sparkbox::Status PlaybackStart(void);

  sparkbox::Status WriteSampleBlock(std::span<int16_t> samples, bool is_mono,
                                    uint32_t sample_rate_hz) final;

  sparkbox::Status PlaybackStop(void) final;

  sparkbox::Status SetOnSampleBlockComplete(Callback& callback) final;

 private:
  Callback on_sample_complete_cb_ = NULL;

  std::fstream file_;
  bool is_mono_;
  uint32_t sample_rate_hz_;
  size_t time_spent_playing_ms_;
};

}  // namespace device::shared::host