#pragma once

#include "error.h"
#include "filesystem/filesystem_driver.h"

namespace Sparkbox::Filesystem {

class FilesystemController {
 public:
  FilesystemController(const FilesystemDriver driver&) :
    driver_(driver) {}

 private:
  const FilesystemDriver driver_;
};

} // namespace Sparkbox::Filesystem