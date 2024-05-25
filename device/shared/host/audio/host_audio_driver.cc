#include "host_audio_driver.h"

#include <chrono>
#include <cstdint>
#include <ctime>
#include <span>
#include <sstream>
#include <string>

#include "sparkbox/assert.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/log.h"
#include "sparkbox/sparkbox.h"
#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
}  // namespace

namespace device::shared::host {

sparkbox::Status HostAudioDriver::PlaybackStart(void) {
  // Playback is just beginning. Open a file to stream audio to
  auto time_now = std::chrono::system_clock::now();
  time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);

  std::ostringstream file_name;
  file_name << std::ctime(&time_now_t) << "_sound_out.wav";

  SP_LOG_INFO("%s", file_name.str().c_str());

  // SP_ASSERT(!file_.is_open());

  return sparkbox::Status::kOk;
}

sparkbox::Status HostAudioDriver::PlaybackStop(void) {
  // SP_ASSERT(file_.is_open());

  // Overwrite the actual header data

  // Close the file

  return sparkbox::Status::kOk;
}

// Write the next block of samples. This is called from an interrupt context.
// For host tests,
sparkbox::Status HostAudioDriver::WriteSampleBlock(std::span<int16_t> samples,
                                                   bool is_mono,
                                                   uint32_t sample_rate_hz) {
  SP_ASSERT(sample_rate_hz != 0);
  // Calculate and save the time spent playing
  time_spent_playing_ms_ =
      samples.size() / (is_mono ? 1 : 2) * 1000 / sample_rate_hz;

  // Call the callback after that amount of time has elapsed
  // sparkbox::Sparkbox::AddTask();

  return sparkbox::Status::kOk;
}

// void HostAudioDriver::WriteSamplesToFile(std::span<int16_t> samples) {}

Status HostAudioDriver::SetOnSampleBlockComplete(Callback& callback) {
  on_sample_complete_cb_ = callback;
  return Status::kOk;
}

}  // namespace device::shared::host