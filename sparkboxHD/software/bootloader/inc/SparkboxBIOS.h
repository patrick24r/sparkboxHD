#pragma once
#include "PeripheralHandles.h"
#include "bootloaderErrors.h"

// Start memory address for level executables
#define LEVEL_START 0x00000000;
// Max size of level executable in bytes
#define MAX_LEVEL_SIZE 0x00000000;

class SparkboxBIOS
{
public:
	
	// Constructor
	SparkboxBIOS();
	
	// Prompts user to select the game on SD card
	int32_t userSelectGame(void);
	
	// Sets the game number of the game currently being played
	void setGame(int32_t gameNum);
	
	// Gets the number of the game being played
	int32_t getGame(void);
	
	// Validate a level for current game
	bootError_t validLevel(int32_t level);
	
	// Validate a level for any game
	bootError_t validLevel(int32_t level, int32_t gameNum);
	
	// Load a level's data into memory
	bootError_t loadLevel(int32_t level);
	
	// Play a level with previously loaded level
	int32_t playLevel(int32_t level);
	
private:
	// Initializes Sparkbox hardware
	int32_t lowLevelInit(void);
	// Number of the selected game. -1 for no game / invalid game
	int32_t currentGameNum;
	// Number of level loaded for selected game. -1 for no level
	int32_t loadedLevelNum;
	// Handles for all hardware peripherals
	PeripheralHandles handles;

};