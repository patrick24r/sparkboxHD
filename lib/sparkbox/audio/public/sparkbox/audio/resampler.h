#pragma once

#include <array>
#include <cstdint>
#include <span>

#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

class Resampler {
 public:
  struct FilterCoefficients {
    float a1;
    float a2;
    float b0;
    float b1;
    float b2;
  };
  struct FilterData {
    float w1;
    float w2;
  };
  struct ResampleFilter {
    uint8_t numerator;
    uint8_t denominator;
    FilterCoefficients coefficients;
    FilterData data[2];

    void ResetData() {
      for (auto& data_it : data) {
        data_it.w1 = 0;
        data_it.w2 = 0;
      }
    }
  };

  template <typename T>
  struct FormattedSamples {
    // num_channels = 1 if mono, 2 if stereo with alternating L R samples
    uint8_t num_channels;
    std::span<T> samples;
    size_t NumberOfBlocks() { return samples.size() / num_channels; }
  };

  // Get a resample filter for a new resample operation
  static ResampleFilter GetResampleFilter(uint8_t ratio_numerator,
                                          uint8_t ratio_denominator);

  // Resample the audio in samples_in to samples_out
  template <typename SampleType>
  static sparkbox::Status ResampleNextBlock(ResampleFilter& filter,
                                            std::span<SampleType> samples_in,
                                            uint8_t num_channels,
                                            std::span<SampleType> samples_out);

 private:
  template <typename T>
  static void BiquadFilter(std::span<T> samples,
                           const FilterCoefficients& coefficients,
                           FilterData& data);

  // Returns the greatest common divisor of the two numbers
  static uint32_t GCD(uint32_t a, uint32_t b) {
    for (uint32_t i = std::min(a, b); i > 0; i--) {
      if (i % a == 0 && i % b == 0) return i;
    }
    return 1;
  }
};

}  // namespace sparkbox::audio