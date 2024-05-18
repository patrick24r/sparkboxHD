#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <string>

#include "sparkbox/audio/imported_file.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

class AudioFileImporter {
 public:
  AudioFileImporter(filesystem::FilesystemDriver &fs_driver)
      : fs_driver_(fs_driver) {}

  sparkbox::Status ImportAudioFiles(const std::string &directory);

  static bool IsSampleRateSupported(uint32_t sample_rate_hz) {
    return sample_rate_hz == 11025 || sample_rate_hz == 22050 ||
           sample_rate_hz == 44100 || sample_rate_hz == 16000 ||
           sample_rate_hz == 48000;
  }

 private:
  struct __attribute__((packed)) WavHeader {
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
  static_assert(sizeof(WavHeader) == 44);

  filesystem::FilesystemDriver &fs_driver_;

  // Abstracted view into imported files
  static constexpr uint32_t kMaxImportedFiles = 256;
  std::map<std::string, ImportedFile> imported_files_;
  uint32_t already_imported_files_ = 0;

  // Underlying memory for the imported samples
  static constexpr uint32_t kMaxBytesPlayback = 1000000U;
  std::array<uint8_t, kMaxBytesPlayback> playback_samples_;
  uint32_t playback_samples_used_bytes_ = 0;

  sparkbox::Status ImportWavFile(const std::string &file);
  sparkbox::Status ImportMp3File(const std::string &file);
};

}  // namespace sparkbox::audio