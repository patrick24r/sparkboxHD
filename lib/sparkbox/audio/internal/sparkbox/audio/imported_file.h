#pragma once

#include <cstdint>

#include "sparkbox/assert.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

// Class holding the data for a single imported file
class ImportedFile {
 public:
  ImportedFile(uint8_t *samples, uint32_t samples_size_bytes,
               uint32_t sample_rate_hz, uint8_t number_of_channels,
               uint8_t bytes_per_sample) {
    SP_ASSERT(samples && bytes_per_sample && number_of_channels);
    samples_ = samples;
    samples_size_bytes_ = samples_size_bytes;
    sample_rate_hz_ = sample_rate_hz;
    number_of_channels_ = number_of_channels;
    bytes_per_sample_ = bytes_per_sample;

    sample_count_ =
        samples_size_bytes_ / number_of_channels_ / bytes_per_sample_;
  }

  uint32_t sample_rate_hz() const { return sample_rate_hz_; }

 private:
  // Raw underlying data
  uint8_t *samples_ = nullptr;
  uint32_t samples_size_bytes_ = 0;

  // Control values
  uint32_t sample_rate_hz_ = 0;
  uint8_t number_of_channels_ = 0;
  uint8_t bytes_per_sample_ = 0;

  // Calculated values
  size_t sample_count_ = 0;
};

}  // namespace sparkbox::audio