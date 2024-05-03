#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

class AudioManager {
 public:
  AudioManager(AudioDriver &driver,
               filesystem::FilesystemDriver &fs_driver) :
               driver_(driver), fs_driver_(fs_driver) {}
  sparkbox::Status ImportAudioFile(const std::string& file);

  void RunTest(void);

  enum SampleRateHz : uint32_t {
    k8k    = 8000,
    k16k   = 16000,
    k22k05 = 22050,
    k44k1  = 44100,
    k48k   = 48000,
  };
  bool IsSampleRateSupported(uint32_t sample_rate_hz) {
    return sample_rate_hz == SampleRateHz::k8k ||
           sample_rate_hz == SampleRateHz::k16k ||
           sample_rate_hz == SampleRateHz::k22k05 ||
           sample_rate_hz == SampleRateHz::k44k1 ||
           sample_rate_hz == SampleRateHz::k48k;
  }

 private:
  #pragma pack(push, 1)
  struct WavHeader {
    char chunk_id[4];
    uint32_t chunk_size;
    char format[4];
    char subchunk_id[4];
    uint32_t subchunk_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t bytes_per_frame;
    uint16_t bits_per_sample;
    char data_id[4];
    uint32_t data_size;
  };
  #pragma pack(pop)
  static_assert(sizeof(WavHeader) == 44);

  // Array of all of the sample data stacked back to back
  static constexpr uint32_t kMaxBytesPlayback = 1000000U;
  std::array<uint8_t, kMaxBytesPlayback> playback_samples_;
  uint32_t playback_samples_used_bytes_ = 0;

  AudioDriver& driver_;
  filesystem::FilesystemDriver& fs_driver_;

  sparkbox::Status ImportWavFile(const std::string& file);
  sparkbox::Status ImportMp3File(const std::string& file);
};


} // namespace Sparkbox::asudio
