#include "sparkbox/audio/resampler.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <span>
#include <vector>

#include "sparkbox/assert.h"
#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
}  // namespace

namespace sparkbox::audio {

Resampler::ResampleFilter Resampler::GetResampleFilter(
    uint8_t ratio_numerator, uint8_t ratio_denominator) {
  SP_ASSERT(ratio_numerator != 0 || ratio_denominator != 0);

  uint32_t gcd = GCD(ratio_denominator, ratio_numerator);
  ratio_denominator /= gcd;
  ratio_numerator /= gcd;

  // Calculate the new coefficients for the 2nd order biquad lowpass filter
  // https://www.st.com/resource/en/application_note/an2874-bqd-filter-design-equations-stmicroelectronics.pdf
  float cutoff_fs =
      1.0 / float(std::max(ratio_numerator, ratio_denominator) * 2);
  float K = tan(cutoff_fs * 3.14159265);
  float W = K * K;
  float Q = 0.707;
  float alpha = 1.0 + K / Q + W;
  return {.numerator = ratio_numerator,
          .denominator = ratio_denominator,
          .coefficients = {.a1 = 2 * (W - 1) / alpha,
                           .a2 = (1 - K / Q + W) / alpha,
                           .b0 = W / alpha,
                           .b1 = 2 * W / alpha,
                           .b2 = W / alpha},
          .data = {
              {
                  .w1 = 0.0,
                  .w2 = 0.0,
              },
              {
                  .w1 = 0.0,
                  .w2 = 0.0,
              },
          }};
}

// Resample audio with the technique described at
// https://en.wikipedia.org/wiki/Sample-rate_conversion
// Allow for the samples to be resampled in place if samples_in and samples_out
// have some overlapping memory
template <typename SampleType>
Status Resampler::ResampleNextBlock(ResampleFilter& filter,
                                    std::span<SampleType> samples_in,
                                    uint8_t num_channels,
                                    std::span<SampleType> samples_out) {
  // If the output buffer is too large, we cannot fill it.
  // (out_num_samples / out_num_channels) <=
  // (in_num_samples / in_num_channels) * (numerator / denominator)
  size_t expected_samples_out =
      samples_in.size() * filter.numerator / filter.denominator;
  if (samples_out.size() > expected_samples_out) {
    SP_LOG_ERROR(
        "Number of output samples too large. Max supported=%zu,  "
        "Actual=%zu",
        expected_samples_out, samples_out.size());
    return Status::kBadParameter;
  }

  if (num_channels > 2) {
    SP_LOG_ERROR("%u channels not supported by resampler", num_channels);
    return Status::kBadParameter;
  }

  // Try to create a working buffer large enough to copy each of the input
  // samples in the format of the output. If the output has two channels, we
  // can split this in half
  auto working_buffer = std::vector<SampleType>(
      samples_out.size() * filter.numerator / num_channels);

  // Go over each channel input (Left and Right)
  for (size_t ch_idx = 0; ch_idx < num_channels; ch_idx++) {
    // For each sample_in, interpolate to 'filter.numerator' samples in the
    // working buffer
    size_t work_buff_idx = 0;
    for (size_t samp_in_idx = ch_idx; samp_in_idx < samples_in.size();
         samp_in_idx += num_channels) {
      // sample_in gets copied to the first spot
      working_buffer[work_buff_idx] = samples_in[samp_in_idx];

      // Then the next (filter.numerator -1) samples are all 0s
      for (int idx_offset = 1; idx_offset < filter.numerator;
           idx_offset++, work_buff_idx++) {
        working_buffer[work_buff_idx] = 0;
      }
    }

    // Filter the working_buffer for the amount of samples we give it
    BiquadFilter<SampleType>(
        std::span<SampleType>(working_buffer.begin(), work_buff_idx + 1),
        filter.coefficients, filter.data[ch_idx]);

    // Copy every filter.denominator'th working_buffer sample to sample_out
    work_buff_idx = filter.denominator - 1;
    for (size_t samp_out_idx = ch_idx; samp_out_idx < samples_out.size();
         samp_out_idx += num_channels) {
      samples_out[samp_out_idx] = working_buffer[work_buff_idx];
      work_buff_idx += filter.denominator;
    }
  }

  return Status::kOk;
}

// Filter the sample data in place
template <typename T>
void Resampler::BiquadFilter(std::span<T> samples,
                             const FilterCoefficients& coefficients,
                             FilterData& data) {
  // Use transposed direct form 2
  for (auto& sample : samples) {
    // cache x(n) for this iteration
    auto current_sample = sample;
    // y(n) = b0 * x(n) + w1(n-1)
    sample = coefficients.b0 * current_sample + data.w1;
    // w1(n) = b1 * x(n) - a1 * y(n) + w2(n-1)
    data.w1 =
        coefficients.b1 * current_sample - coefficients.a1 * sample + data.w2;
    // w2(n) = b2 * x(n) - a2 * y(n)
    data.w2 = coefficients.b2 * current_sample - coefficients.a2 * sample;
  }
}

// Unused function to define valid templates for library compilation
void UnusedFunction() {
  SP_ASSERT("Why are you calling this when I explicitly told you not to");

  Resampler::ResampleFilter filter;
  std::span<int8_t> tmp_span_8;
  std::span<int16_t> tmp_span_16;
  std::span<int32_t> tmp_span_32;
  std::span<int64_t> tmp_span_64;

  Resampler resampler;
  resampler.ResampleNextBlock<int8_t>(filter, tmp_span_8, 1, tmp_span_8);
  resampler.ResampleNextBlock<int16_t>(filter, tmp_span_16, 1, tmp_span_16);
  resampler.ResampleNextBlock<int32_t>(filter, tmp_span_32, 1, tmp_span_32);
  resampler.ResampleNextBlock<int64_t>(filter, tmp_span_64, 1, tmp_span_64);
}

}  // namespace sparkbox::audio