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

  // Reset the "current" sample rate to 0 so when the next samples come, we know
  // to stream to a new file
  current_samples_.sample_rate_hz = 0;

  return sparkbox::Status::kOk;
}

sparkbox::Status HostAudioDriver::PlaybackStop(void) {
  // Overwrite the header data with the actual length

  // Close the file
  sparkbox::Message message =
      sparkbox::Message(sparkbox::MessageType::kAudioStopPlayback);
  SendInternalMessage(message);

  return sparkbox::Status::kOk;
}

// Write the next block of samples. This is called from an interrupt context.
// For host tests, simply write the samples to a file after the amount of time
// to play the samples has elapsed
sparkbox::Status HostAudioDriver::WriteSampleBlock(std::span<int16_t> samples,
                                                   bool is_mono,
                                                   uint32_t sample_rate_hz) {
  SP_ASSERT(sample_rate_hz != 0);

  // Save the new most current samples
  current_samples_ = {
      .samples = samples,
      .is_mono = is_mono,
      .sample_rate_hz = sample_rate_hz,
  };

  // Tell ourselves that we got samples to write on our own task
  sparkbox::Message message =
      sparkbox::Message(sparkbox::MessageType::kAudioStartPlayback);
  SendInternalMessageISR(message);

  return sparkbox::Status::kOk;
}

// Write the samples to the file
void HostAudioDriver::WriteSamplesToFile() {
  // Wait an amount of time the samples should approximately need to play to
  // simulate actual device behavior
  size_t time_spent_playing_ms = current_samples_.samples.size() /
                                 (current_samples_.is_mono ? 1 : 2) * 1000 /
                                 current_samples_.sample_rate_hz;
  vTaskDelay(time_spent_playing_ms);

  // If any parameters changed - sample rate or number of channels - we need to
  // open a new file
  if (previous_samples_.is_mono != current_samples_.is_mono ||
      previous_samples_.sample_rate_hz != current_samples_.sample_rate_hz) {
    SP_LOG_INFO("Detected new audio stream parameters, new file created...");
    CloseOutputFile();
    OpenOutputFile();
  }

  // Write the samples to the open file
  file_.write(reinterpret_cast<char*>(current_samples_.samples.data()),
              current_samples_.samples.size_bytes());
  samples_in_file_count_ += current_samples_.samples.size();

  // Cache the current samples for next time
  previous_samples_ = current_samples_;

  // Tell the sparkbox everything went great
  SP_ASSERT(on_sample_complete_cb_);
  on_sample_complete_cb_(sparkbox::Status::kOk);
}

Status HostAudioDriver::SetOnSampleBlockComplete(Callback& callback) {
  on_sample_complete_cb_ = callback;
  return Status::kOk;
}

void HostAudioDriver::HandleMessage(sparkbox::Message& message) {
  if (message.message_type == sparkbox::MessageType::kAudioStartPlayback) {
    WriteSamplesToFile();
  } else if (message.message_type ==
             sparkbox::MessageType::kAudioStopPlayback) {
    CloseOutputFile();
  } else {
    SP_LOG_ERROR("Unknown message received: %zu",
                 static_cast<size_t>(message.message_type));
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
  std::string file_name = GetOutputFileName();
  if (file_.is_open()) {
    SP_LOG_ERROR("File is already open");
    return;
  }

  file_.open(file_name, std::fstream::out | std::fstream::trunc);

  // Write the WAV header
  WriteWavHeader();
}

void HostAudioDriver::CloseOutputFile() {
  if (!file_.is_open()) {
    SP_LOG_ERROR("File is not open");
    return;
  }

  // Go to the WAV header and rewrite over the data length field
  SP_LOG_INFO("samples_in_file_count_: %u", samples_in_file_count_);
  WriteWavHeader();

  file_.close();
}

// Write the wav file header. Skips
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

  auto stream_pos = file_.tellp();

  // Jump to the front of the file and overwrite the header
  file_.seekp(0);
  file_.write(reinterpret_cast<char*>(&header), sizeof(header));

  // Jump back to where we were before
  file_.seekp(stream_pos);
}

}  // namespace device::shared::host