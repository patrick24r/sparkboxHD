#include <cstdlib>	
#include <iostream>
#include <string>
#include <ctime>


#include "posix_filesystem_driver.h"
#include "sparkbox.h"
#include "sparkbox_device.h"

namespace {
using namespace ::std;
using namespace ::sparkbox::device::posix;
}

int main(void) {
  srand(time(NULL));
  int random_num = std::rand() % 100;
  std::string testText = "Hello World " + std::to_string(random_num) + "!\n";
  PosixFilesystemDriver fs = PosixFilesystemDriver();

  int file = fs.Open("test.txt", kWrite | kCreate);
  fs.Write(file, reinterpret_cast<void*>(const_cast<char*>(testText.data())), testText.length());
  fs.Close(file);
  return 0;
}