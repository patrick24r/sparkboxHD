#include "host_filesystem_driver.h"

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
using sparkbox::filesystem::FilesystemDriver;
}  // namespace

namespace device::shared::host {

Status HostFilesystemDriver::OpenDirectory(int& directory_id,
                                           const std::string& directory) {
  // If we already have too many directories open, do nothing
  if (open_directories_.size() >= kMaxDirs) {
    return Status::kUnavailable;
  }

  if (!Exists(directory)) {
    return Status::kBadResourceState;
  }

  directory_id = GetUniqueDirectoryId();

  auto directory_iterator =
      std::make_unique<std::filesystem::directory_iterator>(directory);

  open_directories_[directory_id] = std::move(directory_iterator);

  return Status::kOk;
}

Status HostFilesystemDriver::CloseDirectory(int directory_id) {
  // If no open directory matches the directory ID, do nothing
  if (!open_directories_.count(directory_id)) return Status::kOk;
  open_directories_.erase(directory_id);

  return Status::kOk;
}

Status HostFilesystemDriver::ReadDirectoryItem(
    int directory_id, FilesystemDriver::DirectoryItem& directory_item) {
  std::filesystem::directory_iterator* iterator =
      open_directories_[directory_id].get();
  const std::filesystem::directory_entry* entry = iterator->operator->();

  if (*iterator == std::filesystem::end(*iterator)) {
    return Status::kBadResourceState;
  }

  directory_item.path = std::string(entry->path());
  directory_item.is_directory = entry->is_directory();
  directory_item.is_regular_file = entry->is_regular_file();

  iterator->operator++();

  return Status::kOk;
}

// Check if a file OR directory exists
bool HostFilesystemDriver::Exists(const std::string& path) {
  return std::filesystem::exists(path);
}

// Create a directory if it does not exist
Status HostFilesystemDriver::CreateDirectory(const std::string& path) {
  if (Exists(path)) {
    return Status::kOk;
  }

  if (!std::filesystem::create_directories(path)) {
    return Status::kUnavailable;
  }

  return Status::kOk;
}

// Remove a file OR directory
Status HostFilesystemDriver::Remove(const std::string& path) {
  // File/folder doesn't exist, no need to do anything
  if (!Exists(path)) {
    return Status::kOk;
  }

  if (!std::filesystem::remove(path)) {
    return Status::kUnavailable;
  }

  return Status::kOk;
}

// Open a file
Status HostFilesystemDriver::Open(int& file_id, const std::string& path,
                                  IoMode mode) {
  // If we already have too many files open, do nothing
  if (open_files_.size() >= kMaxFiles) {
    return Status::kUnavailable;
  }

  if (!Exists(path)) {
    return Status::kBadResourceState;
  }

  file_id = GetUniqueFileId();

  // Open the file in the specified mode
  auto file = std::make_unique<std::fstream>();
  std::ios_base::openmode file_mode = std::fstream::binary;
  if (mode & kRead) file_mode |= std::fstream::in;
  if (mode & kWrite) file_mode |= std::fstream::out;
  if (mode & kCreate) file_mode |= std::fstream::trunc;
  if (mode & kAppend) file_mode |= std::fstream::app;
  file->open(path, file_mode);

  if (file->good()) {
    // Save the open file stream handle and iterate to the next one
    open_files_[file_id] = std::move(file);
    return Status::kOk;
  } else {
    // Failed to open file, maybe try again later???
    return Status::kUnavailable;
  }
}

// Close an open file
Status HostFilesystemDriver::Close(int file_id) {
  // If no open file matches the file ID, do nothing
  if (!open_files_.count(file_id)) return Status::kOk;
  open_files_[file_id]->close();
  open_files_.erase(file_id);
  return Status::kOk;
}

// Read from an open file
Status HostFilesystemDriver::Read(int file_id, void* data, size_t bytes_to_read,
                                  size_t& bytes_read) {
  // Check parameters
  if (data == nullptr) {
    bytes_read = 0;
    return Status::kBadParameter;
  }

  // If no open file matches the file ID, do nothing
  if (!open_files_.count(file_id)) {
    return Status::kBadResourceState;
  }

  // Read from the file
  open_files_[file_id]->read(reinterpret_cast<char*>(data), bytes_to_read);
  bytes_read = open_files_[file_id]->gcount();

  // Don't care about eof or fail, just bad bit
  if (open_files_[file_id]->bad()) {
    return Status::kUnavailable;
  }

  return Status::kOk;
}

// Write to an open file
Status HostFilesystemDriver::Write(int file_id, const void* data,
                                   size_t bytes_to_write,
                                   size_t& bytes_written) {
  // Check parameters
  if (data == nullptr) {
    bytes_written = 0;
    return Status::kBadParameter;
  }

  // If no open file matches the file ID, do nothing
  if (!open_files_.count(file_id)) {
    return Status::kBadResourceState;
  }

  open_files_[file_id]->write(reinterpret_cast<const char*>(data),
                              bytes_to_write);

  // Check write status, don't care about eof
  if (open_files_[file_id]->fail()) {
    bytes_written = 0;
    return Status::kUnavailable;
  }

  bytes_written = bytes_to_write;
  return Status::kOk;
}

}  // namespace device::shared::host