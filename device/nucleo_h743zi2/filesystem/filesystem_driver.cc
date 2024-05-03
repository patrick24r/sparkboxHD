#include "nucleo_h743zi2/filesystem/filesystem_driver.h"

#include "sparkbox/status.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"

namespace {
using ::sparkbox::Status;
} // namespace

namespace nucleoh743zi2 {

Status FilesystemDriver::SetUp(void) {
  return Status::kOk;
}

void FilesystemDriver::TearDown(void) {
  return;
}

bool FilesystemDriver::Exists(const std::string& path) {
  return true;
}

Status FilesystemDriver::CreateDirectory(const std::string& directory) {
  return Status::kOk;
}

Status FilesystemDriver::Remove(const std::string& path) {
  return Status::kOk;
}

Status FilesystemDriver::Open(int& file_id,
                              const std::string& path,
                              IoMode mode) {
  return Status::kOk;
}

Status FilesystemDriver::Close(int file_id) {
  return Status::kOk;
}

Status FilesystemDriver::Read(int file_id,
                              void * data,
                              size_t bytes_to_read,
                              size_t& bytes_read) {
  return Status::kOk;
}

Status FilesystemDriver::Write(int file_id,
                               const void * data,
                               size_t bytes_to_write,
                               size_t& bytes_written) {
  return Status::kOk;
}

} // namespace nucleoh743zi2