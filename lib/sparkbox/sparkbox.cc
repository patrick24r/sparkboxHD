#include "sparkbox/sparkbox.h"

#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace {

using ::sparkbox::Status;

} // namespace

namespace sparkbox {


Status Sparkbox::SetUp(void) {
  Status return_status = Status::kOk;

  // Set up filesystem
  return_status = fs_manager_.SetUp();
  if (return_status != Status::kOk) {
    SP_LOG_ERROR("Error during filesystem set up: %d", static_cast<int>(return_status));
  }

  // Set up controller
  return_status = controller_manager_.SetUp();
  if (return_status != Status::kOk) {
    SP_LOG_ERROR("Error during controller set up: %d", static_cast<int>(return_status));
  }

  return return_status;
}


void Sparkbox::TearDown(void) {
  fs_manager_.TearDown();
}


} // namespace sparkbox