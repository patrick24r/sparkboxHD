#pragma once

#include "sparkbox/filesystem/filesystem_driver.h"

namespace sparkbox::filesystem {

class FilesystemManager {
 public:
  FilesystemManager(FilesystemDriver& driver) :
    driver_(driver) {}

  void RunTest();

 private:
  FilesystemDriver& driver_;
};

} // namespace sparkbox::filesystem