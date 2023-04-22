#include "sparkbox/filesystem/filesystem_manager.h"

#include <string>

#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/log.h"

namespace {
using namespace ::std;
} // namespace

namespace sparkbox::filesystem {

void FilesystemManager::RunTest() {
  char inputBuffer[50];
  std::string testText = "Hello World!";
  sparkbox::Status status;
  int file;
  size_t byte_result;

  SP_LOG_DEBUG("Entered Filesystem test...");

  // Write test
  status = driver_.Open(&file, "test.txt", FilesystemDriver::kWrite | FilesystemDriver::kCreate);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Write test open failed with status: %d", static_cast<int>(status));
    return;
  }

  status = driver_.Write(file,
                    reinterpret_cast<const char*>(testText.data()), testText.length(),
                    &byte_result);
  driver_.Close(file);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Write test write failed with status: %d", static_cast<int>(status));
    return;
  }

  // Read test
  status = driver_.Open(&file, "test.txt", FilesystemDriver::kRead);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Read test open failed with status: %d", static_cast<int>(status));
    return;
  }

  status = driver_.Read(file, inputBuffer, sizeof(inputBuffer), &byte_result);
  driver_.Close(file);

  if (status == Status::kOk) {
    SP_LOG_DEBUG("Read %zu bytes:", byte_result);
    std::string readText(inputBuffer, byte_result);
    SP_LOG_DEBUG("%s", readText.c_str());
  } else {
    SP_LOG_ERROR("Read test open failed with status: %d", static_cast<int>(status));
  }
}

void FilesystemManager::RunDirectoryTest() {
  Status status = driver_.OpenDirectory();
}

} // namespace Sparkbox::Filesystem