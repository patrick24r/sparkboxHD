#include "posix_filesystem_driver.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <memory>

namespace {
using namespace ::std;
} // namespace

namespace sparkbox::device::posix {

int PosixFilesystemDriver::Open(const char * path, IoMode mode) {
  // If the next file id is already open, do nothing.
  // Why are you opening that many files????
  if (open_files_.count(next_file_id_)) {
    std::cout << "Cannot create file, out of file IDs" << std::endl;
    assert(false);
  }

  // Open the file
  auto file = std::make_unique<fstream>();
  std::ios_base::openmode file_mode;
  if (mode & kRead)  file_mode |= std::fstream::in;
  if (mode & kWrite) file_mode |= std::fstream::out;
  if (mode & kCreate) file_mode |= std::fstream::trunc;
  if (mode & kAppend) file_mode |= std::fstream::app;

  file->open(path, file_mode);
  if (file->is_open()) {
    // Save the open file stream handle and iterate to the next one
    int file_id = GetUniqueFileId();
    open_files_[file_id] = std::move(file);
    return file_id;
  } else {
    // Failed to open file, do nothing
    return 0;
  }
}

void PosixFilesystemDriver::Close(int file_id) {
  // If no open file matches the file ID, do nothing
  if (!open_files_.count(file_id)) return;

  open_files_[file_id]->close();
  open_files_.erase(file_id);
  return;
}

size_t PosixFilesystemDriver::Read(int file_id, void * data, size_t data_bytes) {
  return 0;
}

size_t PosixFilesystemDriver::Write(int file_id, void * data, size_t data_bytes) {
  // If no open file matches the file ID, do nothing
  if (!open_files_.count(file_id)) return 0;

  open_files_[file_id]->write(const_cast<const char *>(reinterpret_cast<char*>(data)), data_bytes);

  return data_bytes;
}

} // namespace sparkbox::device::posix