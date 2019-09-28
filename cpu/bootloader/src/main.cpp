#include "main.h"


/**
 * This is the main file for the startup firmware on 
 * the SparkboxHD
 *
 */
 
 
int32_t main()
{
	int32_t gameNum;
	int32_t level = 0;
	SparkboxBIOS spk();
	
	// Continue playing games while able to do so
	while(1) {
		do {
			// User selects the game they want to play
			gameNum = spk.userSelectGame();
			// Error reading SD card, show error and try again
		} while (gameNum < 0);
	
		// Play the next level if it is valid
		while (!spk.validLevel(level)) {
		 
			// Load the level into memory
			if (spk.loadLevel(level)) {
				// Error loading the level, display to user
			
				while(1);
			}
	 
			// Play the level and get the next level
			level = spk.playLevel(level);
		}
	}
}
 