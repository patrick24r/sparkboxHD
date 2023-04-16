#pragma once

#include "sparkbox/filesystem/filesystem_driver.h"

namespace sparkbox::filesystem {

class FilesystemController {
 public:
  FilesystemController(FilesystemDriver& driver) :
    driver_(driver) {}

  void RunTest();

 private:
  FilesystemDriver& driver_;
};

} // namespace sparkbox::filesystem