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

  ImportedFile *GetImportedFile(const std::string &file_name);

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
  std::map<std::string, std::unique_ptr<ImportedFile>> imported_files_;

  sparkbox::Status ImportWavFile(const std::string &file);
  sparkbox::Status ImportMp3File(const std::string &file);
};

}  // namespace sparkbox::audio