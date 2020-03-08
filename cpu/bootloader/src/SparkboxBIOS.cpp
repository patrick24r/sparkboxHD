#include "SparkboxBIOS.h"

// Constructor
void SparkboxBIOS::SparkboxBIOS()
{
	// Initialize state variables

}

// Prompts user to select the game on SD card
std::string SparkboxBIOS::userSelectGame(void)
{
	std::string newGameDir;

	// Find all game folders on SD card

	// Display options to user and let them pick

  // If no new game is picked, return nothing

	return newGameDir;
}

// Validate a level for any game
// Returns a sparkbox error code
int32_t SparkboxBIOS::validLevel(std::string gameDirectory, std::string levelDirectory)
{
	int32_t error = SPK_OK;

	// Check if game folder exists

	// Check if level folder exists in game folder

	// Check for correctly named level file

	// Check if the level code can fit in designated flash memory
	// if (size < MAX_LEVEL_SIZE && size > 0)

	return error;
}

// Load a level's instructions into memory
int32_t SparkboxBIOS::loadLevel(std::string gameDirectory, std::string levelDirectory)
{
	int32_t error = SPK_OK;

	// Get list of game folders and pick one

	// Find level files if they exist in currentGameNum's directory

	// Load it into flash memory

	// Return success or failure of flash memory write

	return error;
}

// Play a level with previously loaded level
std::string SparkboxBIOS::playLevel(Sparkbox *ptr)
{
  std::string nextLevelDirectory;

	// If the code in flash is valid, execute it
	if (levelLoaded) {

    // Unmount file system
    f_mount(0, SDPath, 0);

    // Cast LEVEL_START as a function pointer and call it
    // Return the next level directory
		nextLevelDirectory = ((std::string (*))LEVEL_START)(ptr);

    // Remount file system
    f_mount(&SDFatFs, (TCHAR const*)SDPath, 0);
	}

  return nextLevelDirectory;
}