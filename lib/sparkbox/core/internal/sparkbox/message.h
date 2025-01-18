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

class Message {
 public:
  Message() : Message(MessageType::kMaxMessage) {}
  Message(MessageType msg_type) : Message(msg_type, nullptr, 0) {}
  Message(MessageType msg_type, void* payload_data, size_t payload_data_bytes)
      : message_type(msg_type),
        payload(new uint8_t[payload_data_bytes]),
        payload_bytes(payload_data_bytes) {
    // Copy the payload to this message
    std::memmove(payload, payload_data, payload_data_bytes);
  }

  ~Message() { delete payload; }

  template <typename T>
  T* payload_as() {
    return reinterpret_cast<T*>(payload);
  }

  MessageType message_type;
  uint8_t* payload;
  size_t payload_bytes;
};

}  // namespace sparkbox