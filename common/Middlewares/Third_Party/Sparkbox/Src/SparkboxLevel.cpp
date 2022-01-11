#include "SparkboxLevel.hpp"

SparkboxLevel::SparkboxLevel()
{
	// Initialize audio manager
	audioMgr = new SparkboxAudioManager();

	// Initialize video manager
	videoMgr = new SparkboxVideoManager();
	
	
	//videoMgr->importAllSpriteLayers(levelPath + "/sprite");
}

sparkboxError_t SparkboxLevel::importLevel(string& levelDirectory)
{
	sparkboxError_t status;
	
	status = audioMgr->importAllAudioFiles(levelDirectory + "/audio");
	if (status != SparkboxError::SPARK_OK) return status;

	return SparkboxError::SPARK_OK;
}