#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>

namespace sparkbox::filesystem {

using IoMode = uint8_t;
// kRead : If set, will open the file with read permissions
constexpr IoMode kRead = ((0x01) << 0);
// kWrite : If set, will open the file with write permissions
constexpr IoMode kWrite = ((0x01) << 1);
// kCreate : If set, will always create a new file or overwrite the existing file
constexpr IoMode kCreate = ((0x01) << 2);
// kAppend : If set, will always open a file in append mode
constexpr IoMode kAppend = ((0x01) << 3);

class FilesystemDriver {
 public:

  // File access

  // Open a file. Returns the open file's associated id
  virtual int Open(const char * path, IoMode mode) = 0;
  // Close a file
  virtual void Close(int file_id) = 0;
  // Read from a file
  virtual size_t Read(int file_id, void * data, size_t data_bytes) = 0;
  // Write to a file
  virtual size_t Write(int file_id, void * data, size_t data_bytes) = 0;

  // Directory access
 protected:
  int next_file_id_ = 0;
  int GetUniqueFileId() {
    int file_id = next_file_id_;

    // Keep valid file id's positive
    if (file_id == INT_MAX) {
      next_file_id_ = 0;
    } else {
      ++next_file_id_;
    }
    
    return file_id;
  }

};

} // namespace Sparkbox::Filesystem