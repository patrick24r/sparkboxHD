#pragma once

#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <span>
#include <thread>

#include "device/app/application_driver.h"
#include "sparkbox/assert.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/status.h"

namespace device::host {

class HostAudioDriver : public device::app::AudioAppDriver {
 public:
  HostAudioDriver()
      : write_samples_thread_(&HostAudioDriver::WriteSamplesThreadFn, this) {}

  void SetUp() final;
  void TearDown() final;

  sparkbox::Status PlaybackStart();

  sparkbox::Status WriteSampleBlock(std::span<int16_t> samples,
                                    uint32_t sample_rate_hz) final;

  sparkbox::Status PlaybackStop() final;

  using Callback = sparkbox::audio::AudioDriver::Callback;
  sparkbox::Status SetOnSampleBlockComplete(Callback& callback) final;

 private:
  struct __attribute__((packed)) WavHeader {
    char chunk_id[4] = {'R', 'I', 'F', 'F'};
    uint32_t chunk_size;
    char format[4] = {'W', 'A', 'V', 'E'};
    char subchunk_id[4] = {'f', 'm', 't', ' '};
    uint32_t subchunk_size = 16;  // 16 for PCM
    uint16_t audio_format = 1;    // PCM
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t bytes_per_frame;
    uint16_t bits_per_sample;
    char data_id[4] = {'d', 'a', 't', 'a'};
    uint32_t data_size;
  };

  struct SampleData {
    std::span<int16_t> samples;
    uint32_t sample_rate_hz;
  };

  Callback on_sample_complete_cb_ = NULL;

  std::fstream file_;
  uint32_t samples_in_file_count_;

  SampleData next_samples_;
  SampleData current_samples_;

  std::thread write_samples_thread_;
  bool samples_to_write_;

  void WriteSamplesThreadFn();

  // Functions to gracefully handle writing the audio data to a file that can be
  // listened to later
  std::string GetOutputFileName();
  void OpenOutputFile();
  void WriteWavHeader();
  void WriteNextSamplesToFile();
  void CloseOutputFile();
};

}  // namespace device::host