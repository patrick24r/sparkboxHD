#pragma once

#include <array>
#include <cstdint>
#include <cstring>

namespace sparkbox {

enum class MessageType : size_t {
  // AudioManager Messages
  kAudio = 1000,
  kAudioImportFiles,
  kAudioSetChannelSource,
  kAudioStartPlayback,
  kAudioStopPlayback,
  kAudioBlockComplete,

  // ControllerManager Messages
  kController = 2000,
  kControllerInputChanged,

  // VideoManager Messages
  kVideo = 3000,

  kMaxMessage,
};

// Abstraction around the data we want to send with a message. Note that this
// class takes no ownership of the data it holds
class Message {
 public:
  Message() : Message(MessageType::kMaxMessage) {}
  Message(MessageType msg_type) : Message(msg_type, nullptr, 0) {}
  Message(MessageType msg_type, void* data, size_t data_bytes)
      : message_type_(msg_type), payload_(data), payload_bytes_(data_bytes) {}

  MessageType type() { return message_type_; }
  size_t size_bytes() { return payload_bytes_; }
  template <typename T>
  T* payload_ptr_as() {
    return reinterpret_cast<T*>(payload_);
  }

 private:
  MessageType message_type_;
  void* payload_;
  size_t payload_bytes_;
};

}  // namespace sparkbox