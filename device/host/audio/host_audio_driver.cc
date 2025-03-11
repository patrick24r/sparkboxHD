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
  // Reset the "current" sample rate to 0 so when the next samples come with an
  // actual valid rate, we know to stream to a new file
  current_samples_.sample_rate_hz = 0;
  return sparkbox::Status::kOk;
}

sparkbox::Status HostAudioDriver::PlaybackStop() {
  // Close the file if it is open
  CloseOutputFile();
  return sparkbox::Status::kOk;
}

// Write the next block of samples. For host tests, simply write the samples to
// a file and return after the amount of time to play the samples has elapsed
sparkbox::Status HostAudioDriver::WriteSampleBlock(std::span<int16_t> samples,
                                                   uint32_t sample_rate_hz) {
  SP_ASSERT(sample_rate_hz != 0);

  // Save the new samples as "next" samples
  next_samples_ = {
      .samples = samples,
      .sample_rate_hz = sample_rate_hz,
  };

  // Signal that we're ready to write the samples on our own thread
  samples_to_write_ = true;

  return sparkbox::Status::kOk;
}

Status HostAudioDriver::SetOnSampleBlockComplete(Callback& callback) {
  on_sample_complete_cb_ = callback;
  return Status::kOk;
}

void HostAudioDriver::WriteSamplesThreadFn() {
  while (1) {
    if (samples_to_write_) {
      samples_to_write_ = false;
      WriteNextSamplesToFile();
    } else {
      std::this_thread::yield();
    }
  }
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
  if (file_.is_open()) {
    return;
  }

  std::string file_name = GetOutputFileName();
  file_.open(file_name, std::fstream::out | std::fstream::trunc);
  WriteWavHeader();
}

// Write the wav file header. Maintains the file position
void HostAudioDriver::WriteWavHeader() {
  uint16_t num_channels = 2;
  WavHeader header = {
      .chunk_size = 36 + samples_in_file_count_,
      .num_channels = num_channels,
      .sample_rate = current_samples_.sample_rate_hz,
      .byte_rate = current_samples_.sample_rate_hz * num_channels * 2,
      .bytes_per_frame = static_cast<uint16_t>(num_channels * 2),
      .bits_per_sample = 16,
      .data_size = samples_in_file_count_,
  };

  // Jump to the front of the file and overwrite the header
  file_.seekp(0, std::ios::beg);
  file_.write(reinterpret_cast<char*>(&header), sizeof(header));
}

void HostAudioDriver::WriteNextSamplesToFile() {
  // Writing samples is supposed to take a specified amount of time we need
  // to mimic
  auto time_start = std::chrono::high_resolution_clock::now();
  auto time_end = time_start +
                  std::chrono::milliseconds(next_samples_.samples.size() * 500 /
                                            next_samples_.sample_rate_hz);

  // If the new samples are at a different sample rate, we need a new file
  if (current_samples_.sample_rate_hz != next_samples_.sample_rate_hz) {
    CloseOutputFile();
  }

  current_samples_ = next_samples_;

  // Opens a new output file if one is not already open
  OpenOutputFile();

  file_.write(reinterpret_cast<char*>(current_samples_.samples.data()),
              current_samples_.samples.size());
  samples_in_file_count_ += current_samples_.samples.size();

  // Wait until the samples would be done sending on real hardware
  do {
    std::this_thread::yield();
  } while (std::chrono::high_resolution_clock::now() < time_end);

  SP_ASSERT(on_sample_complete_cb_);
  on_sample_complete_cb_(sparkbox::Status::kOk);
}

void HostAudioDriver::CloseOutputFile() {
  if (!file_.is_open()) {
    return;
  }

  // Go to the WAV header and rewrite over the data length field
  SP_LOG_INFO("samples_in_file_count_: %u", samples_in_file_count_);
  WriteWavHeader();

  samples_in_file_count_ = 0;

  file_.close();
}

}  // namespace device::host