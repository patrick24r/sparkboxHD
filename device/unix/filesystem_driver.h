#pragma once

#include "sparkbox/filesystem/filesystem_driver.h"

namespace {
using ::Sparkbox::Filesystem;
}

namespace Sparkbox::Device::Unix {

class UnixFilesystemDriver : public Filesystem::FilesystemDriver {

};

}