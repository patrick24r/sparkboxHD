#include "nucleo_h743zi2/filesystem/filesystem_driver.h"

#include "nucleo_h743zi2/core/core_defines.h"
#include "sparkbox/status.h"
#include "stm32h7xx_hal.h"

namespace {
using ::sparkbox::Status;
} // namespace

namespace NucleoH743ZI2 {

Status FilesystemDriver::SetUp(void) {
  HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);
  return Status::kOk;
}

void FilesystemDriver::TearDown(void) {
  HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
  return;
}

Status FilesystemDriver::Exists(const std::string& path, bool& exists) {
  return Status::kOk;
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

} // namespace NucleoH743ZI2