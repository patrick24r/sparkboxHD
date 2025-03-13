#include "sparkbox/audio/audio_file_importer.h"

#include <algorithm>
#include <memory>
#include <string>

#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
using sparkbox::filesystem::FilesystemDriver;
} // namespace

namespace sparkbox::audio {

Status AudioFileImporter::ImportAudioFiles(const std::string &directory) {
  // Check if directory exists
  if (!fs_driver_.Exists(directory)) {
    SP_LOG_ERROR("Directory '%s' does not exist", directory.c_str());
    return Status::kUnavailable;
  }

  // Check all files in the directory
  int directory_id;
  FilesystemDriver::DirectoryItem directory_item;
  Status status;

  SP_RETURN_IF_ERROR_LOG(fs_driver_.OpenDirectory(directory_id, directory),
                         "Error opening directory '%s'", directory.c_str());

  while (fs_driver_.ReadDirectoryItem(directory_id, directory_item) ==
         Status::kOk) {
    // Skip any directories or irregular files
    if (!directory_item.is_regular_file) {
      continue;
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

ImportedFile *AudioFileImporter::GetImportedFile(const std::string &file_name) {
  if (imported_files_.count(file_name) != 1) {
    SP_LOG_ERROR("File '%s' not imported", file_name.c_str());
    return nullptr;
  }

  return imported_files_[file_name].get();
}

Status AudioFileImporter::ImportWavFile(const std::string &file_name) {
  // File exists and has correct extension. Try to import it
  Status status;
  int file_id;
  SP_RETURN_IF_ERROR_LOG(
      fs_driver_.Open(file_id, file_name, filesystem::FilesystemDriver::kRead),
      "Error opening wav file");

  // Read the wav header
  WavHeader header;
  size_t bytes_read;
  SP_RETURN_IF_ERROR_LOG(
      fs_driver_.Read(file_id, &header, sizeof(WavHeader), bytes_read),
      "Error reading wav header");
  if (bytes_read != sizeof(WavHeader)) {
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
  } else if (header.bits_per_sample != 32 && header.bits_per_sample != 16 &&
             header.bits_per_sample != 8) {
    SP_LOG_ERROR("Unsupported bits per sample: %d", header.bits_per_sample);
    return Status::kBadResourceState;
  }

  // Copy the header data to an intermediary var because make_unique doesn't
  // like packed structs
  auto data_size = header.data_size;
  auto sample_rate = header.sample_rate;
  auto num_channels = header.num_channels;
  auto bytes_per_sample = header.bits_per_sample / 8;
  imported_files_.insert_or_assign(
      file_name, std::move(std::make_unique<ImportedFile>(
                     data_size, sample_rate, num_channels, bytes_per_sample)));
  status = fs_driver_.Read(file_id, imported_files_[file_name].get()->bytes(),
                           header.data_size, bytes_read);

  if (status != Status::kOk) {
    SP_LOG_ERROR("Error reading wav header: %d", static_cast<int>(status));
    imported_files_.erase(imported_files_.find(file_name));
    return status;
  } else if (bytes_read != header.data_size) {
    SP_LOG_ERROR("Only read %zu, expected %u", bytes_read, header.data_size);
    imported_files_.erase(imported_files_.find(file_name));
    return Status::kBadResourceState;
  }

  return Status::kOk;
}

Status AudioFileImporter::ImportMp3File(const std::string &file_name) {
  // File exists and has correct extension. Try to import it
  SP_LOG_ERROR("Importing mp3 files is currently unsupported");
  return Status::kUnsupported;
}

} // namespace sparkbox::audio