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
  std::string testDirectory = "test_dir";
  std::string testFile = testDirectory + "/test_file.txt";
  sparkbox::Status status;
  int file;
  size_t byte_result;

  SP_LOG_DEBUG("Entered Filesystem test...");

  // Directory test
  status = driver_.CreateDirectory(testDirectory);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Create directory failed with status: %d", static_cast<int>(status));
    return;
  }

  SP_LOG_DEBUG("Created test directory...");

  // Write test
  status = driver_.Open(file, testFile, FilesystemDriver::kWrite | FilesystemDriver::kCreate);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Write test open failed with status: %d", static_cast<int>(status));
    return;
  }

  status = driver_.Write(file,
                    reinterpret_cast<const char*>(testText.data()), testText.length(),
                    byte_result);
  driver_.Close(file);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Write test write failed with status: %d", static_cast<int>(status));
    return;
  }
  SP_LOG_DEBUG("Wrote %zu bytes: '%s'", byte_result, testText.c_str());

  // Read test
  status = driver_.Open(file, testFile, FilesystemDriver::kRead);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Read test open failed with status: %d", static_cast<int>(status));
    return;
  }

  status = driver_.Read(file, inputBuffer, sizeof(inputBuffer), byte_result);
  driver_.Close(file);

  if (status == Status::kOk) {
    std::string readText(inputBuffer, byte_result);
    SP_LOG_DEBUG("Read back %zu bytes: '%s'", byte_result, readText.c_str());
  } else {
    SP_LOG_ERROR("Read test open failed with status: %d", static_cast<int>(status));
    return;
  }

  SP_LOG_DEBUG("Removing files...");

  status = driver_.Remove(testFile);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Remove file failed with status: %d", static_cast<int>(status));
    return;
  }

  status = driver_.Remove(testDirectory);
  if (status != Status::kOk) {
    SP_LOG_ERROR("Remove directory failed with status: %d", static_cast<int>(status));
    return;
  }

  SP_LOG_DEBUG("Exiting filesystem test...");
}

} // namespace Sparkbox::Filesystem