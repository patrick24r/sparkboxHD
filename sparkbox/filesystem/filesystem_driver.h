#pragma once

#include <memory>
#include <string_view>

namespace Sparkbox::Filesystem {

template <typename FileType>
class FilesystemDriver {
 public:
  using FileHandle = std::unique_ptr<FileType>;

  // File access
  virtual FileHandle open(std::string_view path, std::string_view mode);
  virtual void close(FileHandle file);
  virtual Sparkbox::Error read(FileHandle file);
  virtual Sparkbox::Error write(FileHandle file);

  // Directory access
};

} // namespace Sparkbox::Filesystem