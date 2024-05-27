#include "sparkbox/audio/channel.h"

#include <span>

#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
}  // namespace

namespace sparkbox::audio {

Status Channel::SetSource(ImportedFile* file) {
  if (file == nullptr) {
    return sparkbox::Status::kBadParameter;
  } else if (playback_status_ != PlaybackStatus::kStopped) {
    return sparkbox::Status::kBadResourceState;
  }

  audio_source_ = file;
  next_source_sample_index_ = 0;
  repeat_count_ = 0;
  return sparkbox::Status::kOk;
}

Status Channel::SkipToSample(size_t sample_index) {
  if (audio_source_ == nullptr) {
    return sparkbox::Status::kBadResourceState;
  }
  next_source_sample_index_ = sample_index;

  return sparkbox::Status::kOk;
}

Status Channel::SkipToTimeMicroseconds(size_t microseconds) {
  if (audio_source_ == nullptr) {
    return sparkbox::Status::kBadResourceState;
  }
  size_t sample_index =
      microseconds * audio_source_->sample_rate_hz() / 1000000;
  return SkipToSample(sample_index);
}

// Get the next batch of samples for the given audio channel. The samples are
// filled in 16 bit mono format
Status Channel::GetNextSamples(std::span<int16_t> samples, bool is_mono,
                               uint32_t sample_rate) {
  // If a new resampling filter is required due to changed sample rate, get a
  // new filter with proper coefficients. Be careful not to divide by 0
  if (filter_.denominator == 0 ||
      (sample_rate != audio_source_->sample_rate_hz() * filter_.numerator /
                          filter_.denominator)) {
    filter_ = resampler_.GetResampleFilter(audio_source_->sample_rate_hz(),
                                           sample_rate);
  }

  // Keep getting samples until we fill the buffer
  Resampler::FormattedSamples<int16_t> samples_out_format;
  for (size_t sample_filled_idx = 0; sample_filled_idx < samples.size();) {
    // If we hit the end of the file, subtract one from the number of
    // repeats. A negative number of repeats indicates continuous repeat
    if (next_source_sample_index_ >= audio_source_->sample_count()) {
      next_source_sample_index_ = 0;
      if (repeat_count_ > 0) repeat_count_--;
    }

    // If we hit the repeat limit, fill the rest with 0's and return
    if (repeat_count_ == 0) {
      std::fill(samples.subspan(sample_filled_idx).begin(), samples.end(), 0);
      playback_status_ = PlaybackStatus::kStopped;
      return Status::kOk;
    }

    // Try to fill to the end of what is asked for
    size_t samples_to_fill = samples.size() - sample_filled_idx;

    // Find how many samples of the source samples_to_fill equates to
    // (source_samples_to_fill /  source_channels ) * (filter_numerator /
    // filter_denominator) = (samples_to_fill / num_channels)
    size_t source_samples_to_fill =
        (samples_to_fill / (is_mono ? 1 : 2)) * filter_.denominator *
        audio_source_->number_of_channels() / filter_.numerator;

    // Limit the source_samples_to_fill to the end of the source file
    size_t source_samples_remaining =
        audio_source_->sample_count() - next_source_sample_index_;
    if (source_samples_remaining < source_samples_to_fill) {
      source_samples_to_fill = source_samples_remaining;
    }

    // Round up to the next block of samples if necessary
    if (source_samples_to_fill % audio_source_->number_of_channels() != 0) {
      source_samples_to_fill++;
    }

    // We need to give the resampler at least one source block to work with
    source_samples_to_fill =
        std::max(source_samples_to_fill,
                 static_cast<size_t>(audio_source_->number_of_channels()));

    // Update the new number of samples to actually fill
    samples_to_fill =
        (source_samples_to_fill / audio_source_->number_of_channels()) *
        filter_.numerator * (is_mono ? 1 : 2) / filter_.denominator;

    // Do the appropriate resampling
    samples_out_format = {
        .num_channels =
            (is_mono ? static_cast<uint8_t>(1) : static_cast<uint8_t>(2)),
        .samples = samples.subspan(sample_filled_idx, samples_to_fill),
    };
    if (audio_source_->bytes_per_sample() == 1) {
      Resampler::FormattedSamples<int8_t> samples_in_format = {
          .num_channels = audio_source_->number_of_channels(),
          .samples = std::span<int8_t>(
              reinterpret_cast<int8_t*>(audio_source_->samples() +
                                        next_source_sample_index_),
              source_samples_to_fill)};
      resampler_.ResampleNextBlock<int8_t, int16_t>(filter_, samples_in_format,
                                                    samples_out_format);
    } else if (audio_source_->bytes_per_sample() == 2) {
      Resampler::FormattedSamples<int16_t> samples_in_format = {
          .num_channels = audio_source_->number_of_channels(),
          .samples = std::span<int16_t>(
              reinterpret_cast<int16_t*>(audio_source_->samples() +
                                         next_source_sample_index_),
              source_samples_to_fill)};
      resampler_.ResampleNextBlock<int16_t, int16_t>(filter_, samples_in_format,
                                                     samples_out_format);
    } else if (audio_source_->bytes_per_sample() == 4) {
      Resampler::FormattedSamples<int32_t> samples_in_format = {
          .num_channels = audio_source_->number_of_channels(),
          .samples = std::span<int32_t>(
              reinterpret_cast<int32_t*>(audio_source_->samples() +
                                         next_source_sample_index_),
              source_samples_to_fill)};
      resampler_.ResampleNextBlock<int32_t, int16_t>(filter_, samples_in_format,
                                                     samples_out_format);
    }

    // Update the tracking indexes
    next_source_sample_index_ += source_samples_to_fill;
    sample_filled_idx += samples_to_fill;
  }

  return Status::kOk;
}

}  // namespace sparkbox::audio