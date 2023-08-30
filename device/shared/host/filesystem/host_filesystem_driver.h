#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>

#include "sparkbox/status.h"
#include "sparkbox/filesystem/filesystem_driver.h"

namespace {
using ::sparkbox::Status;
} // namespace

namespace device::shared::host {

class HostFilesystemDriver final : public sparkbox::filesystem::FilesystemDriver {
 public:
  static constexpr int kMaxFiles = 5;
  static constexpr int kMaxDirs = 3;
  
  // Nothing special to be done for initialization on host
  Status Init() final { return Status::kOk; }

  Status Exists(const std::string& path, bool& exists) final;
  Status CreateDirectory(const std::string& directory) final;
  Status Remove(const std::string& path) final;

  Status Open(int& file_id, const std::string& path, IoMode mode) final;
  Status Close(int file_id) final;

  Status Read(int file_id,
              void * data,
              size_t bytes_to_read,
              size_t& bytes_read) final;
  Status Write(int file_id,
               const void * data,
               size_t bytes_to_write,
               size_t& bytes_written) final;

 private:
  std::map<int, std::unique_ptr<std::fstream>> open_files_;

  // Gets a unique id for a new file by just picking the next int.
  // If not available, check the next one
  int GetUniqueFileId() {
    static int next_file_id = 0;
    while (open_files_.count(next_file_id)) {
      ++next_file_id;
    }
    return next_file_id;
  }
};

} // namespace device::shared::host