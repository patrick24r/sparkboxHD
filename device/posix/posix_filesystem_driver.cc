#include "posix_filesystem_driver.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <memory>

#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/status.h"

namespace {
using namespace ::std;
using namespace ::sparkbox;
} // namespace

namespace sparkbox::device::posix {

Status PosixFilesystemDriver::Open(int * file_id, 
                                   const char * path,
                                   IoMode mode) {
  // Check for null pointers because safety is important I guess
  if (file_id == nullptr || path == nullptr) {
    return Status::kBadParameter;
  }

  *file_id = GetUniqueFileId();
  // If the file id already exists, you have INT_MAX files open???
  if (open_files_.count(*file_id)) {
    return Status::kUnavailable;
  }

  // Open the file in the specified mode
  auto file = std::make_unique<fstream>();
  std::ios_base::openmode file_mode = std::fstream::binary;
  if (mode & kRead)  file_mode |= std::fstream::in;
  if (mode & kWrite) file_mode |= std::fstream::out;
  if (mode & kCreate) file_mode |= std::fstream::trunc;
  if (mode & kAppend) file_mode |= std::fstream::app;
  file->open(path, file_mode);

  if (file->good()) {
    // Save the open file stream handle and iterate to the next one
    open_files_[*file_id] = std::move(file);
    return Status::kOk;
  } else {
    // Failed to open file, maybe try again later???
    return Status::kUnavailable;
  }
}

Status PosixFilesystemDriver::Close(int file_id) {
  // If no open file matches the file ID, do nothing
  if (!open_files_.count(file_id)) return Status::kOk;

  open_files_[file_id]->close();
  open_files_.erase(file_id);
  return Status::kOk;
}

Status PosixFilesystemDriver::Read(int file_id,
                                   void * data,
                                   size_t bytes_to_read,
                                   size_t * bytes_read) {
  // Check parameters
  if (bytes_read == nullptr) {
    return Status::kBadParameter;
  } else if (data == nullptr) {
    *bytes_read = 0;
    return Status::kBadParameter;
  }

  // If no open file matches the file ID, do nothing
  if (!open_files_.count(file_id)) {
    return Status::kBadResourceState;
  }

  // Read from the file
  *bytes_read = open_files_[file_id]->
      readsome(reinterpret_cast<char*>(data), bytes_to_read);

  // Don't care about eof, just fail and bad bit
  if (open_files_[file_id]->fail()) {
    return Status::kUnavailable;
  }

  return Status::kOk;
}

Status PosixFilesystemDriver::Write(int file_id,
                                    const void * data,
                                    size_t bytes_to_write,
                                    size_t * bytes_written) {
  // Check parameters
  if (bytes_written == nullptr) {
    return Status::kBadParameter;
  } else if (data == nullptr) {
    *bytes_written = 0;
    return Status::kBadParameter;
  }

  // If no open file matches the file ID, do nothing
  if (!open_files_.count(file_id)) {
    return Status::kBadResourceState;
  }

  open_files_[file_id]->write(reinterpret_cast<const char*>(data), bytes_to_write);

  // Check write status
  if (!open_files_[file_id]->good()) {
    *bytes_written = 0;
    return Status::kUnavailable;
  }

  *bytes_written = bytes_to_write;
  return Status::kOk;
}

} // namespace sparkbox::device::posix