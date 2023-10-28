#pragma once

#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/status.h"

namespace nucleoh743zi2 {

class FilesystemDriver final : public sparkbox::filesystem::FilesystemDriver {
 public:
  sparkbox::Status SetUp(void) final;
  void TearDown(void) final;
  
  sparkbox::Status Exists(const std::string& path, bool& exists) final;
  sparkbox::Status CreateDirectory(const std::string& directory) final;
  sparkbox::Status Remove(const std::string& path) final;

  sparkbox::Status Open(int& file_id, const std::string& path, IoMode mode) final;
  sparkbox::Status Close(int file_id) final;
  sparkbox::Status Read(int file_id, void * data,
                        size_t bytes_to_read, size_t& bytes_read) final;
  sparkbox::Status Write(int file_id, const void * data,
                         size_t bytes_to_write, size_t& bytes_written) final;
};

}