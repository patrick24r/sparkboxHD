#pragma once

#include <cstdint>
#include <memory>
#include <span>

#include "sparkbox/assert.h"

namespace sparkbox::audio {

// Class holding the data for a single imported file
class ImportedFile {
 public:
  ImportedFile(uint32_t samples_size_bytes, uint32_t sample_rate_hz,
               uint8_t number_of_channels, uint8_t bytes_per_sample)
      : sample_rate_hz_(sample_rate_hz),
        number_of_channels_(number_of_channels),
        bytes_per_sample_(bytes_per_sample),
        samples_bytes_count_(samples_size_bytes),
        samples_bytes_(std::make_unique<uint8_t[]>(samples_size_bytes)) {
    SP_ASSERT(samples_size_bytes != 0);
    SP_ASSERT(number_of_channels_ != 0);
    SP_ASSERT(bytes_per_sample_ != 0);
  }

  // Control values
  const uint32_t sample_rate_hz_;
  const uint8_t number_of_channels_;
  const uint8_t bytes_per_sample_;
  const uint32_t samples_bytes_count_;

  uint8_t* const bytes() { return samples_bytes_.get(); }
  uint32_t sample_count() {
    return samples_bytes_count_ / bytes_per_sample_ / number_of_channels_;
  }

 private:
  // This class owns the underlying data
  std::unique_ptr<uint8_t[]> samples_bytes_;
};

}  // namespace sparkbox::audio
