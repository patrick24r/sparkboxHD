#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <span>
#include <string>

#include "device/app/application_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace {
using ::sparkbox::Status;
}  // namespace

namespace device::host {

class HostFilesystemDriver final : public device::app::FilesystemAppDriver {
 public:
  static constexpr int kMaxFiles = 5;
  static constexpr int kMaxDirs = 3;

  // Nothing special to be done for initialization on host
  void SetUp(void) { SP_LOG_DEBUG("Host filesystem driver set up..."); }
  void TearDown(void) final {
    SP_LOG_DEBUG("Host filesystem driver tear down...");
  }

  Status OpenDirectory(int& directory_id, const std::string& directory) final;
  Status CloseDirectory(int directory_id) final;
  Status ReadDirectoryItem(
      int directory_id,
      sparkbox::filesystem::FilesystemDriver::DirectoryItem& directory_item)
      final;

  bool Exists(const std::string& path) final;
  Status CreateDirectory(const std::string& directory) final;
  Status Remove(const std::string& path) final;

  using IoMode = sparkbox::filesystem::FilesystemDriver::IoMode;
  Status Open(int& file_id, const std::string& path, IoMode mode) final;
  Status Close(int file_id) final;

  Status Read(int file_id, void* data, size_t bytes_to_read,
              size_t& bytes_read) final;
  Status Write(int file_id, const void* data, size_t bytes_to_write,
               size_t& bytes_written) final;

 private:
  std::map<int, std::unique_ptr<std::fstream>> open_files_;
  std::map<int, std::unique_ptr<std::filesystem::directory_iterator>>
      open_directories_;

  // Gets a unique id for a new file by just picking the next int.
  // If not available, check the next one
  int GetUniqueFileId() {
    static int next_file_id = 0;
    while (open_files_.count(next_file_id)) {
      ++next_file_id;
    }
    return next_file_id;
  }

  // Gets a unique id for a new file by just picking the next int.
  // If not available, check the next one
  int GetUniqueDirectoryId() {
    static int next_dir_id = 0;
    while (open_files_.count(next_dir_id)) {
      ++next_dir_id;
    }
    return next_dir_id;
  }
};

}  // namespace device::host