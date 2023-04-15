#include <cstdlib>	
#include <iostream>
#include <string>
#include <ctime>


#include "posix_filesystem_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/sparkbox.h"
#include "sparkbox/status.h"
#include "sparkbox_device.h"

namespace {
using namespace ::std;
using namespace ::sparkbox::device::posix;
using namespace ::sparkbox::filesystem;
}

int main(void) {
  srand(time(NULL));
  char inputBuffer[500];
  std::string testText = "Hello World " + std::to_string(std::rand() % 10000) + "!";
  PosixFilesystemDriver fs = PosixFilesystemDriver();
  sparkbox::Status status;
  int file;
  size_t byte_result;

  // Write test
  status = fs.Open(&file, "test.txt", FilesystemDriver::kWrite | FilesystemDriver::kCreate);
  if (status != Status::kOk) {
    cout << "Write test open failed with status: "
      << std::to_string(static_cast<int>(status)) << endl;
    return 0;
  }

  status = fs.Write(file,
                    reinterpret_cast<const char*>(testText.data()), testText.length(),
                    &byte_result);
  fs.Close(file);
  if (status != Status::kOk) {
    cout << "Write test write failed." << endl;
    return 0;
  }

  // Read test
  status = fs.Open(&file, "test.txt", FilesystemDriver::kRead);
  if (status != Status::kOk) {
    cout << "Read test open failed with status: "
      << std::to_string(static_cast<int>(status)) << endl;
    return 0;
  }

  status = fs.Read(file, inputBuffer, sizeof(inputBuffer), &byte_result);
  fs.Close(file);

  if (status == Status::kOk) {
    cout << "Read " << std::to_string(byte_result) << " bytes:" << endl;
    std::string readText(inputBuffer, byte_result);
    cout << readText << endl;
  } else {
    cout << "Read test read failed." << endl;
  }

  return 0;
}