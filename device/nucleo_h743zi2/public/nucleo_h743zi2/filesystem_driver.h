#pragma once

#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/status.h"

namespace NucleoH743ZI2 {

class FilesystemDriver final : public sparkbox::filesystem::FilesystemDriver {
 public:
  sparkbox::Status SetUp(void) final { return sparkbox::Status::kOk; }
  void TearDown(void) final {}
  
  sparkbox::Status Exists(const std::string& path, bool& exists) final { return sparkbox::Status::kOk; }
  sparkbox::Status CreateDirectory(const std::string& directory) final { return sparkbox::Status::kOk; }
  sparkbox::Status Remove(const std::string& path) final { return sparkbox::Status::kOk; }

  sparkbox::Status Open(int& file_id, const std::string& path, IoMode mode) final { return sparkbox::Status::kOk; }
  sparkbox::Status Close(int file_id) final { return sparkbox::Status::kOk; }
  sparkbox::Status Read(int file_id, void * data,
                        size_t bytes_to_read, size_t& bytes_read) final { return sparkbox::Status::kOk; }
  sparkbox::Status Write(int file_id, const void * data,
                         size_t bytes_to_write, size_t& bytes_written) final { return sparkbox::Status::kOk; }
};

}