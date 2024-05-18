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
    const uint8_t numerator;
    const uint8_t denominator;
    FilterCoefficients coefficients;
    FilterData data[2];
  };

  template <typename T>
  struct FormattedSamples {
    // num_channels = 1 if mono, 2 if stereo with alternating L R samples
    const uint8_t num_channels;
    std::span<T> samples;

    sparkbox::Status Validate() {
      if (num_channels != 1 && num_channels != 2) {
        SP_LOG_ERROR("Unsupported number of channels: %u", num_channels);
        return sparkbox::Status::kBadParameter;
      }

      return sparkbox::Status::kOk;
    }
  };

  // Get a resample filter for a new resample operation
  ResampleFilter GetResampleFilter(uint8_t ratio_numerator,
                                   uint8_t ratio_denominator);

  template <typename InType, typename OutType>
  // Resample the audio in samples_in to samples_out
  sparkbox::Status ResampleNextBlock(ResampleFilter& filter,
                                     FormattedSamples<InType>& samples_in,
                                     FormattedSamples<OutType>& samples_out);

 private:
  template <typename T>
  void BiquadFilter(std::span<T> samples,
                    const FilterCoefficients& coefficients, FilterData& data);
};

}  // namespace sparkbox::audio