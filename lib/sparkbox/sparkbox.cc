#include "sparkbox/sparkbox.h"

#include "freertos/task.h"
#include "sparkbox/log.h"
#include "sparkbox/status.h"
#include "sparkbox/task.h"

namespace {

using ::sparkbox::Status;

}  // namespace

namespace sparkbox {

Status Sparkbox::SetUp(void) {
  // Set up controller and its task
  SP_RETURN_IF_ERROR_LOG(controller_manager_.SetUp(),
                         "Error during controller set up");

  // Set up audio and its task
  SP_RETURN_IF_ERROR_LOG(audio_manager_.SetUp(), "Error during audio set up");

  return Status::kOk;
}

void Sparkbox::Start(void) {
  // Add an entry task to load and run the os
  entry_task_.AddToScheduler(this);

  vTaskStartScheduler();

  // If we somehow get here, the scheduler has stopped. Remove the entry task to
  // properly clean up
  entry_task_.RemoveFromScheduler();
}

void Sparkbox::EntryTaskWrapper(void* sparkbox_ptr) {
  SP_ASSERT(sparkbox_ptr != nullptr);
  static_cast<Sparkbox*>(sparkbox_ptr)->EntryTask();
}

void Sparkbox::EntryTask() {
  // Load and run the os, which allows the user to select the game
  const char* next_level = core_manager_.LoadAndRunLevel(
      "../../lib/sparkbox/level/os/libsparkbox.level.os.so");

  // Now that a game has been selected, that game should continuously pick new
  // levels to run
  while (1) {
    next_level = core_manager_.LoadAndRunLevel(next_level);
    if (next_level == nullptr) {
      // The level did not specify a next level
      SP_LOG_ERROR("Level '%s' did not specify a next level, aborting...",
                   next_level);
      break;
    }
  }

  while (1) {
    taskYIELD();
  }
}

void Sparkbox::TearDown(void) {
  // Tear down in the opposite order as set up
  audio_manager_.TearDown();
  controller_manager_.TearDown();
}

}  // namespace sparkbox