#include "host_audio_driver.h"

#include <chrono>
#include <cstdint>
#include <ctime>
#include <span>
#include <sstream>
#include <string>
#include <thread>

#include "sparkbox/assert.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
}  // namespace

namespace device::host {

void HostAudioDriver::SetUp() { SP_LOG_DEBUG("Host audio driver set up..."); }

void HostAudioDriver::TearDown() {
  SP_LOG_DEBUG("Host audio driver tear down...");
}

sparkbox::Status HostAudioDriver::PlaybackStart() {
  // Playback is just beginning. Open a file to stream audio data to
  OpenOutputFile();
  // Reset the "current" sample rate to 0 so when the next samples come, we know
  // to stream to a new file
  current_samples_.sample_rate_hz = 0;
  return sparkbox::Status::kOk;
}

sparkbox::Status HostAudioDriver::PlaybackStop() {
  // Close the file
  CloseOutputFile();
  return sparkbox::Status::kOk;
}

// Write the next block of samples. For host tests, simply write the samples to
// a file and return after the amount of time to play the samples has elapsed
sparkbox::Status HostAudioDriver::WriteSampleBlock(std::span<int16_t> samples,
                                                   bool is_mono,
                                                   uint32_t sample_rate_hz) {
  SP_ASSERT(sample_rate_hz != 0);

  // If we have a new sample rate or are switching between mono and stereo, we
  // need to stream to a new file

  // Save the new most current samples
  current_samples_ = {
      .samples = samples,
      .is_mono = is_mono,
      .sample_rate_hz = sample_rate_hz,
  };

  return sparkbox::Status::kOk;
}

Status HostAudioDriver::SetOnSampleBlockComplete(Callback& callback) {
  on_sample_complete_cb_ = callback;
  return Status::kOk;
}

std::string HostAudioDriver::GetOutputFileName() {
  auto time_now = std::chrono::system_clock::now();
  time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);

  char date_format[25] = "YYYY-mm-dd_HH-MM-SS";
  strftime(date_format, sizeof(date_format), "%Y-%m-%d_%H-%M-%S",
           std::localtime(&time_now_t));
  std::string ret_str = "playback_" + std::string(date_format) + ".wav";

  return ret_str;
}

void HostAudioDriver::OpenOutputFile() {
  std::string file_name = GetOutputFileName();
  if (file_.is_open()) {
    return;
  }

  file_.open(file_name, std::fstream::out | std::fstream::trunc);
  WriteWavHeader();
}

// Write the wav file header. Maintains the file position
void HostAudioDriver::WriteWavHeader() {
  uint16_t num_channels = (current_samples_.is_mono ? 1 : 2);
  WavHeader header = {
      .chunk_size = 36 + samples_in_file_count_,
      .num_channels = num_channels,
      .sample_rate = current_samples_.sample_rate_hz,
      .byte_rate = current_samples_.sample_rate_hz * num_channels * 2,
      .bytes_per_frame = static_cast<uint16_t>(num_channels * 2),
      .data_size = samples_in_file_count_,
  };

  // Jump to the front of the file and overwrite the header
  file_.seekp(0);
  file_.write(reinterpret_cast<char*>(&header), sizeof(header));
}

void HostAudioDriver::CloseOutputFile() {
  if (!file_.is_open()) {
    return;
  }

  // Go to the WAV header and rewrite over the data length field
  SP_LOG_INFO("samples_in_file_count_: %u", samples_in_file_count_);
  WriteWavHeader();

  file_.close();
}

}  // namespace device::host