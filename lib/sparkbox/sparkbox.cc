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
  // Set up core and its task
  SP_RETURN_IF_ERROR_LOG(core_manager_.SetUp(), "Error during core set up");

  // Set up controller and its task
  SP_RETURN_IF_ERROR_LOG(controller_manager_.SetUp(),
                         "Error during controller set up");

  // Set up audio and its task
  SP_RETURN_IF_ERROR_LOG(audio_manager_.SetUp(), "Error during audio set up");

  return Status::kOk;
}

void Sparkbox::Start(void) {
  // Add an entry task.
  vTaskStartScheduler();
}

void Sparkbox::TearDown(void) {
  // Tear down in the opposite order as set up
  audio_manager_.TearDown();
  controller_manager_.TearDown();
}

}  // namespace sparkbox