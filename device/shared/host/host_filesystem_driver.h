#pragma once

#include <cstdint>
#include <fstream>
#include <limits>
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

class HostFilesystemDriver : public FilesystemDriver {
 public:
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

  // Get a unique file id for a new file by picking an int
  // one higher than the last item in open_files
  int GetUniqueFileId() {
    if (open_files_.empty()) {
      return INT_MIN;
    }

    return (open_files_.rbegin()->first + 1);
  }
};

} // namespace device::shared::host