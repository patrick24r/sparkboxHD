#include "sparkbox/audio/audio_file_importer.h"

#include <algorithm>
#include <span>
#include <string>

#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
using sparkbox::filesystem::FilesystemDriver;
}  // namespace

namespace sparkbox::audio {

Status AudioFileImporter::ImportAudioFiles(const std::string& directory) {
  // Check if directory exists
  if (!fs_driver_.Exists(directory)) {
    SP_LOG_ERROR("Directory '%s' does not exist", directory.c_str());
    return Status::kUnavailable;
  }

  // Check all files in the directory
  int directory_id;
  FilesystemDriver::DirectoryItem directory_item;
  Status status;

  status = fs_driver_.OpenDirectory(directory_id, directory);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Error opening directory '%s': %d", directory.c_str(),
                 static_cast<int>(status));
    return status;
  }

  while (fs_driver_.ReadDirectoryItem(directory_id, directory_item) ==
         Status::kOk) {
    // Skip any directories or irregular files
    if (!directory_item.is_regular_file) {
      continue;
    }

    // Check if we can import any more files
    if (already_imported_files_ == kMaxImportedFiles) {
      SP_LOG_ERROR(
          "Cannot import '%s', reached limit for number of imported files",
          directory_item.path.c_str());
      break;
    }

    // Search for file extension
    std::string::size_type idx = directory_item.path.rfind('.');
    if (idx == std::string::npos) {
      SP_LOG_WARN("File '%s' extension not found", directory_item.path.c_str());
      continue;
    }

    // Extension found, check extension to determine import type
    std::string extension = directory_item.path.substr(idx + 1);
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (extension == "wav") {
      status = ImportWavFile(directory_item.path);
    } else if (extension == "mp3") {
      status = ImportMp3File(directory_item.path);
    } else {
      SP_LOG_INFO("Unsupported audio file extension '%s'", extension.c_str());
      status = Status::kOk;
    }

    // Hit an error trying to import this file. Reset the error status and try
    // to import the next file
    if (status != Status::kOk) {
      SP_LOG_ERROR("Error importing audio file '%s'",
                   directory_item.path.c_str());
      status = Status::kOk;
    }
  }

  fs_driver_.CloseDirectory(directory_id);

  return Status::kOk;
}

Status AudioFileImporter::ImportWavFile(const std::string& file_name) {
  // File exists and has correct extension. Try to import it
  Status status;
  int file_id;
  status =
      fs_driver_.Open(file_id, file_name, filesystem::FilesystemDriver::kRead);
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
  } else if (header.num_channels != 1 && header.num_channels != 2) {
    SP_LOG_ERROR("Unsupported number of channels: %u", header.num_channels);
    return Status::kBadResourceState;
  } else if (header.chunk_size != header.data_size + 36) {
    SP_LOG_ERROR("Invalid chunk or data size");
    return Status::kBadResourceState;
  } else if (header.bits_per_sample != 32 && header.bits_per_sample != 16 &&
             header.bits_per_sample != 8) {
    SP_LOG_ERROR("Unsupported bits per sample: %d", header.bits_per_sample);
    return Status::kBadResourceState;
  }

  // Import the audio if there is still space for it
  if (kMaxBytesPlayback - playback_samples_used_bytes_ < header.data_size) {
    SP_LOG_ERROR("Not enough space to import '%s'", file_name.c_str());
    return Status::kBadResourceState;
  }

  // Save the sample data
  uint8_t* file_data_location =
      playback_samples_.data() + playback_samples_used_bytes_;
  status = fs_driver_.Read(file_id, file_data_location, header.data_size,
                           bytes_read);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Error reading wav header: %d", static_cast<int>(status));
    return status;
  } else if (bytes_read != header.data_size) {
    SP_LOG_ERROR("Only read %lu, expected %u", bytes_read, header.data_size);
    // No need to clear the data we did read. It will never be played or will
    // be overwritten
    return Status::kBadResourceState;
  }
  playback_samples_used_bytes_ += header.data_size;

  // Save the sample metadata mapped to the file name
  imported_files_.insert_or_assign(
      file_name,
      ImportedFile(file_data_location, header.data_size, header.sample_rate,
                   header.num_channels, header.bits_per_sample / 8));
  already_imported_files_++;

  SP_LOG_INFO("Imported '%s' for %dB, %uB/%uB available)", file_name.c_str(),
              header.data_size,
              kMaxBytesPlayback - playback_samples_used_bytes_,
              kMaxBytesPlayback);
  return Status::kOk;
}

Status AudioFileImporter::ImportMp3File(const std::string& file_name) {
  // File exists and has correct extension. Try to import it
  SP_LOG_ERROR("Importing mp3 files is currently unsupported");
  return Status::kUnsupported;
}
}  // namespace sparkbox::audio