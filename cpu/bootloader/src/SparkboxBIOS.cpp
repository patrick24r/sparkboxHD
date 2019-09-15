#include "SparkboxBIOS.h"

// Constructor
void SparkboxBIOS::SparkboxBIOS()
{
	// Initialize state variables
	// No current valid game number
	currentGameNum = -1;
	// No current valid level number loaded into flash
	loadedLevelNum = -1;

	// Initialize Sparkbox hardware
	handles = PeripheralHandles();
}
	
// Prompts user to select the game on SD card
int32_t SparkboxBIOS::userSelectGame(void)
{
	currentGameNum = -1;
	// Find all game folders on SD card
	
	// Display options to user and let them pick
	
	return currentGameNum;
}
	
// Sets the game number of the game currently being played
void SparkboxBIOS::setGame(int32_t gameNum)
{
	currentGameNum = gameNum;
}
	
// Gets the number of the game being played
int32_t SparkboxBIOS::getGame(void)
{
	return currentGameNum;
}
	
// Helper function to validate a level for current game
bootError_t SparkboxBIOS::validLevel(int32_t level)
{
	return validLevel(level, currentGameNum);
}
	
// Validate a level for any game
// Returns 0 for valid level
// Returns other error code for invalid level
bootError_t SparkboxBIOS::validLevel(int32_t level, int32_t gameNum)
{
	bootError_t error = NO_ERROR;

	// Check if game folder exists
	
	// Check if level folder exists in game folder
	
	// Check for correctly named level file
	
	// Check if the level code can fit in designated flash memory
	// if (size < MAX_LEVEL_SIZE && size > 0) 
	
	return error;
}
	
// Load a level's data into memory
// Returns 0 on success
// Returns -1 on failure
bootError_t SparkboxBIOS::loadLevel(int32_t level)
{
	bootError_t error = NO_ERROR;

	// Get list of game folders and pick one specified by currentGameNum

	// Find level files if they exist in currentGameNum's directory
	
	// Load it into flash memory
	
	// Return success or failure of flash memory write
	
	if (error) loadedLevelNum = -1;
	else loadedLevelNum = level;

	return error;
	
}
	
// Play a level with previously loaded level
int32_t SparkboxBIOS::playLevel(int32_t level)
{
	// If the code in flash is valid, execute it
	if (loadedLevelNum >= 0 && loadedLevelNum == level) {
		// Call level with peripheral handles
		// Return the next level number

		// Cast LEVEL_START as a function pointer that takes a pointer to a 
		// PeripheralHandles class as an argument. Then pass it the address
		// to the instance of the PeripheralHandles class named 'handles'
		return ((int32_t(*)(&PeripheralHandles))LEVEL_START)(&handles);
	}
}