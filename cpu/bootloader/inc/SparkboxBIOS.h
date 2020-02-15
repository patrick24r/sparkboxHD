#pragma once
#include "Sparkbox.h"
#include "SparkboxError.h"
#include <string>

// Start memory address for level executables
#define LEVEL_START 0x00000000;
// Max size of level executable in bytes
#define MAX_LEVEL_SIZE 0x00000000;

class SparkboxBIOS
{
public:
  // Constructor and Destructor
  SparkboxBIOS();
  ~SparkboxBIOS();

  // Prompts user to select the game on SD card
  std::string userSelectGame(void);

  // Validate a level on disk
  int32_t validLevel(std::string gameDirectory, std::string levelDirectory);

  // Load a level's instructions into memory
  int32_t loadLevel(std::string gameDirectory, std::string levelDirectory);

  // Play a level with previously loaded level
  std::string playLevel(Sparkbox *ptr);
private:
	// 1 if a level is loaded, 0 if no level is loaded
  int32_t levelLoaded;
};
