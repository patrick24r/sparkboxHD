#include "SparkboxLevel.hpp"

SparkboxLevel::SparkboxLevel()
{
	// Initialize audio manager
	audioMgr = new SparkboxAudioManager();

	// Initialize video manager
	videoMgr = new SparkboxVideoManager();
	
	//audioMgr->importAllAudioFiles(levelPath + "/audio");
	//videoMgr->importAllSpriteLayers(levelPath + "/sprite");
}

sparkboxError_t SparkboxLevel::importLevel(string& levelDirectory)
{
	return SparkboxError::SPARK_OK;
}