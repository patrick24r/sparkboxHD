#include "sparkbox/level/os/os.h"

#include <string>

#include "sparkbox/level/level_lib.h"
#include "sparkbox/log.h"

extern "C" sparkbox::level::LevelInterface* CreateLevel(
    sparkbox::SparkboxLevelInterface& sparkbox) {
  return new sparkbox::level::os::Os(sparkbox);
}

extern "C" void DestroyLevel(sparkbox::level::LevelInterface* sparkbox) {
  if (sparkbox) {
    delete sparkbox;
  }
}

namespace sparkbox::level::os {

const char* Os::Run() {
  SP_LOG_DEBUG("Started Sparkbox OS!");
  // Load assets
  sparkbox_.Audio().ImportAudioFiles("sparkbox/os/sounds");
  // sparkbox_.Video().ImportSprites("sparkbox/os/sprites");

  // Play the audio once on channel 0
  sparkbox_.Audio().SetChannelAudioSource(
      0, "sparkbox/os/sounds/sample-file-3.wav");
  sparkbox_.Audio().PlayAudio(0, 1);

  while (1) {
  }
}

}  // namespace sparkbox::level::os