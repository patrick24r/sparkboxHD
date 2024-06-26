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
  Status return_status = Status::kOk;

  // Set up core peripherals
  return_status = core_driver_.SetUp();
  if (return_status != Status::kOk) {
    SP_LOG_ERROR("Error during core set up: %d",
                 static_cast<int>(return_status));
  }

  // Set up filesystem
  return_status = fs_manager_.SetUp();
  if (return_status != Status::kOk) {
    SP_LOG_ERROR("Error during filesystem set up: %d",
                 static_cast<int>(return_status));
  }

  // Set up controller
  return_status = controller_manager_.SetUp();
  if (return_status != Status::kOk) {
    SP_LOG_ERROR("Error during controller set up: %d",
                 static_cast<int>(return_status));
  }

  // Set up audio
  return_status = audio_manager_.SetUp();
  if (return_status != Status::kOk) {
    SP_LOG_ERROR("Error during audio set up: %d",
                 static_cast<int>(return_status));
  }

  return return_status;
}

void Sparkbox::Start(void) {
  vTaskStartScheduler();
  // Should never reach here but have it anyways
  while (1) {
  }
}

void Sparkbox::TearDown(void) {
  // Tear down in the opposite order as set up
  audio_manager_.TearDown();
  controller_manager_.TearDown();
  fs_manager_.TearDown();
  core_driver_.TearDown();
}

}  // namespace sparkbox