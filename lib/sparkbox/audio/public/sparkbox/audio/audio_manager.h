#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "FreeRTOS.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/audio/audio_file_importer.h"
#include "sparkbox/status.h"
#include "task.h"

namespace sparkbox::audio {

class AudioManager {
 public:
  AudioManager(AudioDriver &driver, filesystem::FilesystemDriver &fs_driver)
      : driver_(driver), audio_file_importer_(fs_driver) {}

  sparkbox::Status SetUp(void);
  void TearDown(void);

  // Set the audio file source for a channel. Stops
  sparkbox::Status SetChannelAudioSource(int channel, std::string &audio_file) {
    return sparkbox::Status::kOk;
  }

 private:
  AudioDriver &driver_;
  AudioFileImporter audio_file_importer_;

  // Audio task data
  constexpr static size_t kTaskStackDepth = configMINIMAL_STACK_SIZE;
  constexpr static size_t kTaskPriority = 5;
  static_assert(kTaskStackDepth >= configMINIMAL_STACK_SIZE);
  TaskHandle_t task_handle_;
  static void AudioTaskWrapper(void *audioManager);
  void AudioTask(void);
};

}  // namespace sparkbox::audio
