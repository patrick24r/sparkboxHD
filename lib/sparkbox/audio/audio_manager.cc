#include "sparkbox/audio/audio_manager.h"

#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
}


namespace sparkbox::audio {

Status ImportAudioFiles(const char *directory) {
  // Open directory
  return Status::kOk;
}

Status ImportAudioFile(const char *file) {
  // Check file suffix
  return Status::kOk;
}

Status ImportWavFile(const char *file) {
  return Status::kOk;
}

Status ImportMp3File(const char *file) {
  return Status::kOk;
}

} // namespace sparkbox::audio