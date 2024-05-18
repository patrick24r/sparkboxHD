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

  // Calculate the new coefficients for the 2nd order biquad lowpass filter
  // https://www.st.com/resource/en/application_note/an2874-bqd-filter-design-equations-stmicroelectronics.pdf
  float cutoff_fs =
      1.0 / float(std::max(ratio_numerator, ratio_denominator) * 2);
  float K = tan(cutoff_fs * 3.14159265);
  float W = K * K;
  float Q = 0.707;
  float alpha = 1.0 + K / Q + W;
  ResampleFilter filter_out = {.numerator = ratio_numerator,
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
  return filter_out;
}

// Resample audio with the technique described at
// https://en.wikipedia.org/wiki/Sample-rate_conversion
// Allow for the samples to be resampled in place if samples_in and samples_out
// have some overlapping memory
template <typename InType, typename OutType>
Status Resampler::ResampleNextBlock(ResampleFilter& filter,
                                    FormattedSamples<InType>& samples_in,
                                    FormattedSamples<OutType>& samples_out) {
  SP_RETURN_IF_ERROR(samples_in.Validate());
  SP_RETURN_IF_ERROR(samples_out.Validate());

  // Check that the output buffer is the correct amount of samples
  // (out_num_samples / out_num_channels) =
  // (in_num_samples / in_num_channels) * (numerator / denominator)
  size_t expected_output_samples =
      (samples_in.samples.size() * filter.numerator *
       samples_out.num_channels) /
      (samples_in.num_channels * filter.denominator);
  if (expected_output_samples != samples_out.samples.size()) {
    SP_LOG_ERROR(
        "Output samples not the correct size. Expected=%zu, Actual=%zu",
        expected_output_samples, samples_out.num_samples);
    return Status::kBadParameter;
  }

  // Try to create a working buffer large enough to copy each of the input
  // samples in the format of the output. If the output has two channels, we can
  // split this in half
  auto working_buffer =
      std::vector<OutType>(samples_in.samples.size() * filter.numerator *
                           sizeof(OutType) / samples_out.num_channels);

  // For each channel output (Left and Right)
  for (size_t ch_idx = 0; ch_idx < samples_out.num_channels; ch_idx++) {
    // Iterate over the samples_in and copy them to the intermediate
    // working_buffer_
    size_t work_buff_idx = 0;
    for (size_t samp_in_idx = ch_idx; samp_in_idx < samples_in.samples.size();
         samp_in_idx += samples_in.num_channels) {
      // For each sample_in, interpolate to (filter.numerator - 1) samples
      for (int idx_offset = 0; idx_offset < filter.numerator; idx_offset++) {
        // For the first sample of each filter.numerator chunk of samples, fill
        // it with the actual next sample. Otherwise, it's a zero
        if (idx_offset == 0) {
          // Copy the sample at this index into the working buffer at this index
          InType SampleIn
        } else {
          working_buffer[work_buff_idx] = 0;
        }
        work_buff_idx++;
      }
    }

    // Filter the working_buffer for the amount of samples we give it
    BiquadFilter(std::span<OutType>(working_buffer.begin(), work_buff_idx + 1),
                 filter.coefficients, filter.data[ch_idx]);

    // Copy every filter.denominator'th working_buffer_ sample to sample_out
    work_buff_idx = filter.denominator - 1;
    for (size_t samp_out_idx = ch_idx; samp_out_idx < samples_out.size();
         samp_out_idx += num_channels) {
      samples_out.samples[samp_out_idx] = working_buffer[work_buff_idx];
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
    T current_sample = sample;
    // y(n) = b0 * x(n) + w1(n-1)
    sample = coefficients.b0 * current_sample + data.w1;
    // w1(n) = b1 * x(n) - a1 * y(n) + w2(n-1)
    data.w1 =
        coefficients.b1 * current_sample - coefficients.a1 * sample + data.w2;
    // w2(n) = b2 * x(n) - a2 * y(n)
    data.w2 = coefficients.b2 * current_sample - coefficients.a2 * sample;
  }
}

}  // namespace sparkbox::audio