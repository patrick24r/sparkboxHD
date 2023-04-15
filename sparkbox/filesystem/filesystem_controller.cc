#include "sparkbox/filesystem/filesystem_controller.h"

#include "sparkbox/filesystem/filesystem_driver.h"

#include <string>
#include <iostream>

namespace {
using namespace ::std;
} // namespace

namespace sparkbox::filesystem {

void FilesystemController::RunTest() {
  char inputBuffer[500];
  std::string testText = "Hello World!";
  sparkbox::Status status;
  int file;
  size_t byte_result;

  // Write test
  status = driver_.Open(&file, "test.txt", FilesystemDriver::kWrite | FilesystemDriver::kCreate);
  if (status != Status::kOk) {
    cout << "Write test open failed with status: "
      << std::to_string(static_cast<int>(status)) << endl;
    return;
  }

  status = driver_.Write(file,
                    reinterpret_cast<const char*>(testText.data()), testText.length(),
                    &byte_result);
  driver_.Close(file);
  if (status != Status::kOk) {
    cout << "Write test write failed." << endl;
    return;
  }

  // Read test
  status = driver_.Open(&file, "test.txt", FilesystemDriver::kRead);
  if (status != Status::kOk) {
    cout << "Read test open failed with status: "
      << std::to_string(static_cast<int>(status)) << endl;
    return;
  }

  status = driver_.Read(file, inputBuffer, sizeof(inputBuffer), &byte_result);
  driver_.Close(file);

  if (status == Status::kOk) {
    cout << "Read " << std::to_string(byte_result) << " bytes:" << endl;
    std::string readText(inputBuffer, byte_result);
    cout << readText << endl;
  } else {
    cout << "Read test read failed." << endl;
  }
}

} // namespace Sparkbox::Filesystem