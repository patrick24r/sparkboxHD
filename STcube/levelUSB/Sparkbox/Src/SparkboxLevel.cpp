#include "SparkboxLevel.h"

SparkboxLevel::SparkboxLevel(string levelPath)
{
	
	// Initialize audio manager
	audioMgr = new SparkboxAudioManager();
	audioMgr->importAllAudioFiles(levelPath, audioFileIDs);
	// Add the audio management thread
	audioManagerTask_attributes.name = "audioManagerTask";
	audioManagerTask_attributes.stack_size = 256 * 4;
	audioManagerTask_attributes.priority = osPriorityHigh;
	audioManagerHandle = osThreadNew(audioMgr->threadfcn_AudioManager, 
		(void*)audioMgr, &audioManagerTask_attributes);


	// Initialize video manager

	// Add the video management thread
}
