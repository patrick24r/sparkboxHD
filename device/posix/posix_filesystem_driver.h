#pragma once

#include <cstdint>
#include <fstream>
#include <map>
#include <memory>

#include "sparkbox/filesystem/filesystem_driver.h"

namespace {
using namespace ::std;
using namespace ::sparkbox::filesystem;
} // namespace

namespace sparkbox::device::posix {

class PosixFilesystemDriver : public filesystem::FilesystemDriver {
 public:
  using IoMode = filesystem::IoMode;
  int Open(const char * path, IoMode mode);
  void Close(int file_id);
  size_t Read(int file_id, void * data, size_t data_bytes);
  size_t Write(int file_id, void * data, size_t data_bytes);

 private:
  int next_file_id_ = 0;
  std::map<int, std::unique_ptr<fstream>> open_files_;
};

} // namespace sparkbox::device::posix