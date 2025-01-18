#pragma once

#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/filesystem/filesystem_manager_interface.h"

namespace sparkbox::filesystem {

class FilesystemManager : public FilesystemManagerInterface {
 public:
  FilesystemManager(FilesystemDriver& driver) : driver_(driver) {}

  void RunTest(void) final;

 private:
  FilesystemDriver& driver_;
};

}  // namespace sparkbox::filesystem