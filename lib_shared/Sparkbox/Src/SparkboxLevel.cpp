#include "SparkboxLevel.h"

SparkboxLevel::SparkboxLevel(string levelPath)
{
	
	// Initialize audio manager
	audioMgr = new SparkboxAudioManager();
	//audioMgr->importAllAudioFiles(levelPath + "/audio");


	// Initialize video manager
	videoMgr = new SparkboxVideoManager();
	//videoMgr->importAllSpriteLayers(levelPath + "/sprite");
}
