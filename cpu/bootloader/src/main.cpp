#include "main.h"

// Global handle to all sparkbox peripherals
// This is global to have access to it during interrupts
Sparkbox *spk;

/**
 * This is the main file for the startup firmware on
 * the SparkboxHD
 */
int32_t main()
{
  // Default file paths to BIOS on disk
  std::string gameDirectory = "bios";
  std::string levelDirectory = "init_game";

  // Initialize the sparkbox and all peripherals
  // while allocating memory for all handles
  spk = new Sparkbox();

bios_start:
  // Initialize the BIOS
  SparkboxBIOS bios = SparkboxBIOS();
  // Import all level data for bios
  spk->importLevel(gameDirectory + "/" + levelDirectory);

  // Continue playing games while able to do so
  while (1) {
    do {
      // User selects the game they want to play
      gameDirectory = bios.userSelectGame();
      // Error reading SD card, show error and try again
    } while (gameDirectory.empty());

    // Play the next level if it is valid
    while (bios.validLevel(gameDirectory, levelDirectory) == SPK_OK) {

      // Load the level executable into flash memory
      if (bios.loadLevel(gameDirectory, levelDirectory) == SPK_OK) {
        // Error loading the level, display to user
        while(1);
		// Will restart BIOS if error in future versions
		goto bios_start;
      }

      // Play the level and get the next level
      levelDirectory = bios.playLevel(spk);
	}
}
