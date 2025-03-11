#pragma once

#include <cstdint>
#include <functional>
#include <span>

#include "sparkbox/status.h"

namespace sparkbox::audio {

class AudioDriver {
 public:
  using Callback = std::function<void(sparkbox::Status)>;

  virtual ~AudioDriver() = default;

  // Device specific function called when playback begins
  virtual sparkbox::Status PlaybackStart() = 0;

  // Non-blocking call to write a block of samples. This is called from an
  // interrupt context
  virtual sparkbox::Status WriteSampleBlock(std::span<int16_t> samples,
                                            uint32_t sample_rate) = 0;

  // Device specific function called when playback stops
  virtual sparkbox::Status PlaybackStop() = 0;

  // Sets the callback for when the last block of samples finishes playing
  virtual sparkbox::Status SetOnSampleBlockComplete(Callback& callback) = 0;
};

}  // namespace sparkbox::audio