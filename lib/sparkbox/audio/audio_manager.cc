#include "sparkbox/audio/audio_manager.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>

#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
}


namespace sparkbox::audio {

void AudioManager::RunTest(void) {
  ImportAudioFile("CantinaBand3.wav");
}


Status AudioManager::ImportAudioFile(const std::string& file) {
  // Check if file exists
  if (!fs_driver_.Exists(file)) {
    SP_LOG_ERROR("Audio file '%s' does not exist", file.c_str());
    return Status::kUnavailable;
  }

  // Search for file extension
  std::string::size_type idx = file.rfind('.');
  if (idx == std::string::npos) {
    SP_LOG_ERROR("Audio file '%s' extension not found", file.c_str());
    return Status::kBadParameter;
  }
  
  // Extension found, check extension to determine import type
  std::string extension = file.substr(idx + 1);
  // Convert to lowercase for comparison
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if (extension == "wav") {
    return ImportWavFile(file);
  } else if (extension == "mp3") {
    return ImportMp3File(file);
  } else {
    SP_LOG_ERROR("Audio file unsupported extension '%s'", extension.c_str());
    return Status::kBadParameter;
  }
}

Status AudioManager::ImportWavFile(const std::string& file_name) {
  // File exists and has correct extension. Try to import it
  Status status;
  int file_id;
  status = fs_driver_.Open(file_id, file_name, filesystem::FilesystemDriver::kRead);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Error opening wav file");
    return status;
  }

  // Read the wav header
  WavHeader header;
  size_t bytes_read;
  status = fs_driver_.Read(file_id, &header, sizeof(WavHeader), bytes_read);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Error reading wav header: %d", static_cast<int>(status));
    return status;
  } else if (bytes_read != sizeof(WavHeader)) {
    SP_LOG_ERROR("wav header not fully read, is the file too short?");
    return Status::kBadResourceState;
  }

  if (std::string(header.chunk_id, 4).compare("RIFF") ||
      std::string(header.format, 4).compare("WAVE") ||
      std::string(header.subchunk_id, 4).compare("fmt ") ||
      std::string(header.data_id, 4).compare("data")) {
    SP_LOG_ERROR("Invalid header ID");
    return Status::kBadResourceState;
  } else if (header.subchunk_size != 16 || header.audio_format != 1) {
    SP_LOG_ERROR("Only PCM is supported for wav files");
    return Status::kBadResourceState;
  } else if (!IsSampleRateSupported(header.sample_rate)) {
    SP_LOG_ERROR("Unsupported sample rate: %uHz", header.sample_rate);
    return Status::kBadResourceState;
  } else if (header.chunk_size != header.data_size + 36) {
    SP_LOG_ERROR("Invalid chunk or data size");
    return Status::kBadResourceState;
  } else if (header.bits_per_sample != 16 && header.bits_per_sample != 8) {
    SP_LOG_ERROR("Unsupported bits per sample: %d", header.bits_per_sample);
    return Status::kBadResourceState;
  }

  // Import the audio if there is still space for it
  if (kMaxBytesPlayback - playback_samples_used_bytes_ < header.data_size) {
    SP_LOG_ERROR("Not enough space to import wav file");
    return Status::kBadResourceState;
  }
  
  // Read the rest of the data in
  status = fs_driver_.Read(file_id,
      playback_samples_.data() + playback_samples_used_bytes_,
      header.data_size, bytes_read);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Error reading wav header: %d", static_cast<int>(status));
    return status;
  } else if (bytes_read != header.data_size) {
    SP_LOG_ERROR("Only read %lu, expected %u", bytes_read, header.data_size);
    // No need to clear the data we did read. It will never be played or will be
    // overwritten
    return Status::kBadResourceState;
  }

  return Status::kOk;
}

Status AudioManager::ImportMp3File(const std::string& file_name) {
  // File exists and has correct extension. Try to import it
  return Status::kOk;
}

} // namespace sparkbox::audio