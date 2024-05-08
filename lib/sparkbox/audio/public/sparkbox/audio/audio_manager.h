#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string>

#include "FreeRTOS.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/audio/audio_file_importer.h"
#include "sparkbox/manager.h"
#include "sparkbox/message.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

class AudioManager : sparkbox::Manager {
 public:
  AudioManager(AudioDriver &driver, filesystem::FilesystemDriver &fs_driver)
      : sparkbox::Manager(kConfig),
        driver_(driver),
        audio_file_importer_(fs_driver) {}

  sparkbox::Status SetUp(void) override;
  void TearDown(void) override;

  // Set the audio file source for a channel. Stops
  sparkbox::Status SetChannelAudioSource(int channel, const char *audio_file) {
    return sparkbox::Status::kOk;
  }

 private:
  static constexpr sparkbox::Manager::Config kConfig = {
      .task_name = "AudioTask",
      .task_stack_depth = configMINIMAL_STACK_SIZE,
      .task_priority = 5,
      .queue_length = 50,
  };
  AudioDriver &driver_;
  AudioFileImporter audio_file_importer_;

  // Max allowable latency is linked to video FPS. At 60 Hz, period = 16.7 ms
  // Ensure we have enough buffer space for 2 buffers of 15 ms of 16 bit, dual
  // channel audio at max sample rate. This buffer will be filled with pre-mixed
  // audio. Audio will be 16 bit
  // 48 kHz * kBufferDepthMs ms * 2 channels * 2 buffers
  static constexpr size_t kBufferDepthMs = 15;
  static constexpr size_t kMixedSampleBufferSizeBytes =
      48 * kBufferDepthMs * 2 * 2;
  std::array<int16_t, kMixedSampleBufferSizeBytes> mixed_samples_buffers_;

  void HandleMessage(sparkbox::Message &message) override;
  void BlockCompleteCb(sparkbox::Status status);

};  // namespace sparkbox::audio

}  // namespace sparkbox::audio
