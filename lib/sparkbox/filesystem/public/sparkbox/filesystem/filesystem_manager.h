#pragma once

#include "sparkbox/filesystem/filesystem_driver.h"

namespace sparkbox::filesystem {

class FilesystemManager {
 public:
  FilesystemManager(FilesystemDriver& driver) : driver_(driver) {}

  Status SetUp(void) { return sparkbox::Status::kOk; }
  void TearDown(void) {}

  void RunTest(void);

 private:
  FilesystemDriver& driver_;
};

}  // namespace sparkbox::filesystem