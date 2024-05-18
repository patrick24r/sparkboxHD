#pragma once

namespace sparkbox {

enum class MessageType : size_t {
  // AudioManager Messages
  kAudio = 1000,
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

struct Message {
  Message()
      : message_type(MessageType::kMaxMessage),
        payload(nullptr),
        payload_size(0) {}
  Message(MessageType msg_type)
      : message_type(msg_type), payload(nullptr), payload_size(0) {}
  Message(MessageType msg_type, void* payload_data, size_t payload_data_size)
      : message_type(msg_type),
        payload(payload_data),
        payload_size(payload_data_size) {}

  MessageType message_type;
  void* payload;
  size_t payload_size;
};

}  // namespace sparkbox