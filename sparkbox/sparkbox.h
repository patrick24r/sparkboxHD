#pragma once

#include "sparkbox/filesystem/filesystem_controller.h"
#include "sparkbox/filesystem/filesystem_driver.h"

namespace {
using namespace ::sparkbox::filesystem;
}

namespace sparkbox {

class Sparkbox final {
 public:
  Sparkbox(filesystem::FilesystemDriver& fs_driver) :
    fs_controller_{fs_driver} {}

  const filesystem::FilesystemController& 
    GetFilesystemController() { return fs_controller_; }

 private:
  filesystem::FilesystemController fs_controller_;
};

} // namespace Sparkbox