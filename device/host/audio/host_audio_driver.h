#pragma once

#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <span>

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/manager.h"
#include "sparkbox/status.h"

namespace device::host {

class HostAudioDriver : public sparkbox::audio::AudioDriver,
                        public sparkbox::Manager {
 public:
  HostAudioDriver() : sparkbox::Manager("HostAudioDriver") {}

  sparkbox::Status SetUp(void) final {
    sparkbox::Manager::SetUp();
    return sparkbox::Status::kOk;
  }
  void TearDown(void) final { sparkbox::Manager::TearDown(); }

  sparkbox::Status PlaybackStart(void);

  sparkbox::Status WriteSampleBlock(std::span<int16_t> samples, bool is_mono,
                                    uint32_t sample_rate_hz) final;

  sparkbox::Status PlaybackStop(void) final;

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
    uint16_t bits_per_sample = 16;
    char data_id[4] = {'d', 'a', 't', 'a'};
    uint32_t data_size;
  };

  Callback on_sample_complete_cb_ = NULL;

  std::fstream file_;
  uint32_t samples_in_file_count_;
  std::string GetOutputFileName();

  struct SampleData {
    std::span<int16_t> samples;
    bool is_mono = true;
    uint32_t sample_rate_hz;
  };
  SampleData previous_samples_;
  SampleData current_samples_;

  void HandleMessage(sparkbox::Message& message) final;

  // Functions to gracefully
  void OpenOutputFile();
  void WriteWavHeader();
  void WriteSamplesToFile();
  void CloseOutputFile();
};

}  // namespace device::host