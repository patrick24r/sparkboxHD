#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "sparkbox/status.h"

namespace {
using ::sparkbox::Status;
}  // namespace

namespace sparkbox::filesystem {

class FilesystemDriver {
 public:
  using IoMode = int;
  // kRead : If set, will open the file with read permissions
  static constexpr IoMode kRead = 1 << 0;
  // kWrite : If set, will open the file with write permissions
  static constexpr IoMode kWrite = 1 << 1;
  // kCreate : If set, will always create a new file or overwrite the existing
  // file
  static constexpr IoMode kCreate = 1 << 2;
  // kAppend : If set, will always open a file in append mode
  static constexpr IoMode kAppend = 1 << 3;

  // Checks if a file or directory exists
  virtual bool Exists(const std::string& path) = 0;
  // Creates a directory
  virtual Status CreateDirectory(const std::string& directory) = 0;
  // Removes a file or directory
  virtual Status Remove(const std::string& path) = 0;

  // Directory operations
  struct DirectoryItem {
    std::string path;
    bool is_regular_file;
    bool is_directory;
  };
  // Open a directory
  virtual Status OpenDirectory(int& directory_id,
                               const std::string& directory) = 0;
  // Close a directory
  virtual Status CloseDirectory(int directory_id) = 0;
  // Read the next directory item in an open directory. Returns
  // Statusd::kBadResourceState on reaching the end of the directory
  virtual Status ReadDirectoryItem(int directory_id,
                                   DirectoryItem& directory_item) = 0;

  // File operations
  // Open a file. Populates the file_id on success
  virtual Status Open(int& file_id, const std::string& path, IoMode mode) = 0;
  // Close a file
  virtual Status Close(int file_id) = 0;
  // Read from a file. Populates bytes_read with the number of bytes read
  virtual Status Read(int file_id, void* data, size_t bytes_to_read,
                      size_t& bytes_read) = 0;
  // Write to a file. Populates bytes_written with the number of bytes written
  virtual Status Write(int file_id, const void* data, size_t bytes_to_write,
                       size_t& bytes_written) = 0;
};

}  // namespace sparkbox::filesystem