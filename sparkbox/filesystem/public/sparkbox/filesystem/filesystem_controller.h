#pragma once

#include "error.h"
#include "sparkbox/filesystem/filesystem_driver.h"

namespace sparkbox::filesystem {

class FilesystemController {
 public:
  FilesystemController(const FilesystemDriver& driver) :
    driver_(driver) {}

 private:
  const FilesystemDriver& driver_;
};

} // namespace Sparkbox::Filesystem