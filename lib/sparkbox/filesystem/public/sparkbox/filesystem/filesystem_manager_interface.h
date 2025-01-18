#pragma once

extern "C" {
namespace sparkbox::filesystem {

class FilesystemManagerInterface {
 public:
  virtual void RunTest() = 0;
};

}  // namespace sparkbox::filesystem
}