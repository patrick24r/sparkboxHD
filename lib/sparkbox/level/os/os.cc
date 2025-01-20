#include "sparkbox/level/os/os.h"

#include "sparkbox/level/level.h"
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
  // sparkbox_.Audio().ImportAudioFiles("os/sounds");
  // sparkbox_.Video().ImportSprites("os/sprites");

  // Play the audio once
  // sparkbox_.Audio().SetChannelAudioSource(0, "cantinaband.wav");
  // sparkbox_.Audio().PlayAudio(0, 1);

  return nullptr;
}

}  // namespace sparkbox::level::os