#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string>

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/audio/audio_file_importer.h"
#include "sparkbox/audio/audio_manager_interface.h"
#include "sparkbox/audio/stream.h"
#include "sparkbox/manager.h"
#include "sparkbox/message.h"
#include "sparkbox/router.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

class AudioManager : public AudioManagerInterface, sparkbox::Manager {
 public:
  AudioManager(Router &router, AudioDriver &driver,
               filesystem::FilesystemDriver &fs_driver)
      : sparkbox::Manager(router, Destination::kAudioManager),
        driver_(driver),
        audio_file_importer_(fs_driver) {}

  sparkbox::Status SetUp(void);
  void TearDown(void);

  sparkbox::Status ImportAudioFiles(const std::string &directory) final;
  sparkbox::Status SetStreamAudioSource(uint8_t stream,
                                        const char *audio_file) final;
  sparkbox::Status PlayAudio(uint8_t stream, int number_of_repeats) final;
  sparkbox::Status StopAudio(uint8_t stream) final;

 private:
  // Max allowable latency is linked to video FPS since we want each new frame
  // to sync with new audio changes. At 60 Hz, period = 16.7 ms. Ensure we have
  // enough buffer space for 2 buffers of 15 ms of 16 bit, dual channel audio at
  // max sample rate.
  static constexpr size_t kNumMixedBuffers = 2;
  static constexpr size_t kMixedBufferDepthMs = 15;
  static constexpr size_t kMaxSampleRatekHz = 48;
  static constexpr size_t kMaxChannels = 2;

  // Allow for up to 4 concurrent streams
  static constexpr uint8_t kMaxStreams = 4;

  // Total required 16 bit samples for a mixed buffer of 15 ms:
  // (max sample rate khz) * (max latency ms) * (max channels)
  static constexpr size_t kMixedBufferDepthSamples =
      kMaxSampleRatekHz * kMixedBufferDepthMs * kMaxChannels;

  using SparkboxStream = Stream<kMixedBufferDepthSamples>;

  // Class representing a buffer of mixed samples ready to be sent to the driver
  class MixedAudioBuffer {
   public:
    // Returns how many samples of the buffer are used. The buffer will always
    // be 15 ms, so this will change based on sample rate
    // buffer (ms) * sample_rate (kHz) * 2 channels
    size_t UsedSamplesSize() {
      return kMixedBufferDepthMs * sample_rate_hz_ / 1000 * 2;
    }

    // Array of fully mixed samples
    std::array<int16_t, kMixedBufferDepthSamples> mixed_samples_;
    uint32_t sample_rate_hz_ = 0;
    bool is_ready_;
  };

  AudioDriver &driver_;
  AudioFileImporter audio_file_importer_;

  std::array<SparkboxStream, kMaxStreams> audio_streams_;

  std::array<std::array<int16_t, kMixedBufferDepthSamples>, kMaxStreams>
      unmixed_samples_;

  // Fully mixed samples buffers. While one is actively being used, the other is
  // being mixed
  std::array<MixedAudioBuffer, kNumMixedBuffers> mixed_samples_buffers_;
  int next_buffer_idx_;

  void HandleMessage(sparkbox::Message &message) override;
  void HandleImportAudioFiles(const char *directory);
  void HandleAudioStartPlayback(uint8_t stream, int number_of_repeats);
  void HandleAudioStopPlayback(uint8_t stream);
  void HandleAudioBlockComplete();
  void HandleAudioSetStreamSource(uint8_t stream, const char *audio_file);

  bool AnyStreamPlaying() {
    for (auto &stream_it : audio_streams_) {
      if (stream_it.GetPlaybackStatus() ==
          SparkboxStream::PlaybackStatus::kPlaying) {
        return true;
      }
    }
    return false;
  }

  void MixNextBuffer();
  void WriteNextBuffer();

  void BlockCompleteCb(sparkbox::Status status);
};

}  // namespace sparkbox::audio
