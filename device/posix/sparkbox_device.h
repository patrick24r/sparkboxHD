#pragma once

#include "posix_filesystem_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox.h"


namespace {
using namespace ::sparkbox;
using namespace ::sparkbox::device::posix;
}

namespace sparkbox::device {

sparkbox::Sparkbox& GetSparkbox() {
  static posix::PosixFilesystemDriver fs_driver;
  static sparkbox::Sparkbox sbox = Sparkbox(fs_driver);
  return sbox;
}

}