#pragma once

#include <cstdint>
#include <fstream>
#include <map>
#include <memory>

#include "sparkbox/status.h"
#include "sparkbox/filesystem/filesystem_driver.h"

namespace {
using namespace ::sparkbox::filesystem;
using namespace ::sparkbox;
using namespace ::std;
} // namespace

namespace device::shared::host {

class HostFilesystemDriver final : public FilesystemDriver {
 public:
  static constexpr int kMaxFiles = 5;
  Status Open(int * file_id, const char * path, IoMode mode) final;
  Status Close(int file_id) final;

  Status Read(int file_id,
              void * data,
              size_t bytes_to_read,
              size_t * bytes_read) final;
  Status Write(int file_id,
               const void * data,
               size_t bytes_to_write,
               size_t * bytes_written) final;

 private:
  std::map<int, std::unique_ptr<std::fstream>> open_files_;

  int next_file_id_ = 0;
  // Gets a unique file id for a new file by just picking the next int.
  // If not available, check the next one
  int GetUniqueFileId() {
    while (open_files_.count(next_file_id_)) {
      ++next_file_id_;
    }

    return next_file_id_;
  }
};

} // namespace device::shared::host