#include "host_audio_driver.h"

#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
}  // namespace

namespace device::shared::host {

Status HostAudioDriver::SetOnSampleBlockComplete(Callback& callback) {
  on_sample_complete_cb_ = callback;
  return Status::kOk;
}

}  // namespace device::shared::host