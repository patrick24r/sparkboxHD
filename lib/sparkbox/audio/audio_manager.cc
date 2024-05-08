#include "sparkbox/audio/audio_manager.h"

#include <cstdint>
#include <cstring>

#include "FreeRTOS.h"
#include "sparkbox/assert.h"
#include "sparkbox/log.h"
#include "sparkbox/manager.h"
#include "sparkbox/status.h"
#include "task.h"

namespace {
using sparkbox::Message;
using sparkbox::MessageType;
using sparkbox::Status;
using std::placeholders::_1;
}  // namespace

namespace sparkbox::audio {

Status AudioManager::SetUp(void) {
  SP_ASSERT(driver_.SetUp() == Status::kOk);
  SP_ASSERT(Manager::SetUp() == Status::kOk);

  AudioDriver::Callback cb =
      std::bind(&AudioManager::BlockCompleteCb, this, _1);
  // Import all audio from the "sounds" directory
  return audio_file_importer_.ImportAudioFiles("sounds");
}

void AudioManager::TearDown(void) {
  Manager::TearDown();
  driver_.TearDown();
}

// Dispatch a message. Guaranteed to be on the audio manager's task
void AudioManager::HandleMessage(Message &message) {
  if (message.message_type == MessageType::kAudioBlockComplete) {
    // Fill the next block with mixed audio
  } else {
    SP_LOG_INFO("Unknown message type received by audio manager: %zu",
                static_cast<size_t>(message.message_type));
  }
}

// Last block of audio was sent by the device driver
void AudioManager::BlockCompleteCb(Status status) {
  // A block just finished sending
  Message message = Message(MessageType::kAudioBlockComplete);
  SendInternalMessageISR(message);

  // If we're still playing, send the next block
  // Assert that the next block is ready
  // driver_.WriteSampleBlock();
}

}  // namespace sparkbox::audio