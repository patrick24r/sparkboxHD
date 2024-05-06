#include "sparkbox/audio/audio_manager.h"

#include <cstdint>
#include <cstring>

#include "FreeRTOS.h"
#include "sparkbox/log.h"
#include "sparkbox/status.h"
#include "task.h"

namespace {
using sparkbox::Status;
}

namespace sparkbox::audio {

Status AudioManager::SetUp(void) {
  Status status;

  // Set up the audio manager task
  SP_ASSERT(xTaskCreate(AudioTaskWrapper, "AudioTask", kTaskStackDepth, this,
                        kTaskPriority, &task_handle_) == pdPASS);

  // Import all audio from the "sounds" directory
  status = audio_file_importer_.ImportAudioFiles("sounds");

  return status;
}

void AudioManager::TearDown(void) {
  // Tear down the audio manager task
  if (task_handle_ != nullptr) {
    vTaskDelete(task_handle_);
  }
}

void AudioManager::AudioTaskWrapper(void* audio_manager) {
  SP_ASSERT(audio_manager != nullptr);
  AudioManager* manager = static_cast<AudioManager*>(audio_manager);
  manager->AudioTask();
}

void AudioManager::AudioTask(void) {
  while (1) {
    // Task yield at the top of the loop so continue statements hit it
    taskYIELD();

    // If we're not playing audio, continue

    // Update the timers
  }
}

}  // namespace sparkbox::audio