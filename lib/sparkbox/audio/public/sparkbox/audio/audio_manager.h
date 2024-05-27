#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string>

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/audio/audio_file_importer.h"
#include "sparkbox/audio/channel.h"
#include "sparkbox/manager.h"
#include "sparkbox/message.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

class AudioManager : sparkbox::Manager {
 public:
  AudioManager(AudioDriver &driver, filesystem::FilesystemDriver &fs_driver)
      : sparkbox::Manager("AudioTask"),
        driver_(driver),
        audio_file_importer_(fs_driver) {}

  sparkbox::Status SetUp(void);
  void TearDown(void);

  // Set the audio file source for a channel. Stops
  sparkbox::Status SetChannelAudioSource(uint8_t channel,
                                         const char *audio_file);
  sparkbox::Status PlayAudio(uint8_t channel, int number_of_repeats);
  sparkbox::Status StopAudio(uint8_t channel);

 private:
  AudioDriver &driver_;
  AudioFileImporter audio_file_importer_;

  static constexpr uint8_t kMaxChannels = 4;
  std::array<Channel, kMaxChannels> audio_channel_;

  // Max allowable latency is linked to video FPS. At 60 Hz, period = 16.7
  // ms Ensure we have enough buffer space for 2 buffers of 15 ms of 16 bit,
  // dual channel audio at max sample rate. This buffer will be filled with
  // pre-mixed audio. Audio will be 16 bit 48 kHz * kBufferDepthMs ms * 2
  // channels

  // Ensure we have 2 buffers of mixed audio + 4 buffers of intermediate audio
  // for resampling each of the 4 channels on the fly
  static constexpr size_t kNumberOfBuffers = 2;
  // Buffer depth in milliseconds
  static constexpr size_t kBufferDepthMs = 15;
  // Number of 16 bit samples in each of the kNumberOfBuffers buffers
  static constexpr size_t kMixedSampleBufferSize = 48 * kBufferDepthMs * 2;
  std::array<std::array<int16_t, kMixedSampleBufferSize>, kNumberOfBuffers>
      mixed_samples_buffer_;

  std::array<std::array<int16_t, kMixedSampleBufferSize>, kMaxChannels>
      unmixed_samples_buffer_;

  struct BufferParameters {
    // If false, there is no audio to send
    bool any_channel_playing = false;
    // true if the next buffer is properly mixed
    bool is_ready = false;
    // Either 0 or 1 to indicate the index of mixed_samples_buffer_
    int buffer_index = 0;

    // Parameters used to send the samples to play to the driver
    uint32_t sample_rate_hz = 0;
    bool is_mono = false;
    uint32_t buffer_size_samples = 0;
  };
  BufferParameters next_buffer_;

  void HandleMessage(sparkbox::Message &message) override;
  void HandleAudioStartPlayback(uint8_t channel, int number_of_repeats);
  void HandleAudioStopPlayback(uint8_t channel);
  void HandleAudioBlockComplete(void);
  void HandleAudioSetChannelSource(uint8_t channel, const char *audio_file);

  bool AnyChannelPlaying(void) {
    for (uint8_t ch_idx = 0; ch_idx < kMaxChannels; ch_idx++) {
      if (audio_channel_[ch_idx].GetPlaybackStatus() ==
          Channel::PlaybackStatus::kPlaying) {
        return true;
      }
    }
    return false;
  }
  void MixNextSampleBlock(void);
  void WriteNextSampleBlock(void);

  void BlockCompleteCb(sparkbox::Status status);
};

}  // namespace sparkbox::audio
