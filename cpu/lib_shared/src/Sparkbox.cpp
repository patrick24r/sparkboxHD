#include "Sparkbox.h"

Sparkbox::Sparkbox(std::string *levelDirectory)
{
  int32_t error;

  // Save the file path to the root directory of the current level
  rootLevelDirectory = *levelDirectory;
  

  // TODO: Have input arguments define pins maybe?
  // That would make this WAY more flexible
  gpu = SparkboxGpu();
  audio = SparkboxAudio();
  controller = SparkboxController();
}

// Delete all Sparkbox allocated memory
Sparkbox::~Sparkbox()
{
  // Free all memory that was allocated
}

// Import sprites in rootDirectory into cpu and gpu memory
int32_t Sparkbox::importLevel(void)
{
  gpu.addAllSprites(rootLevelDirectory);
  audio.addAllAudio(rootLevelDirectory);
}