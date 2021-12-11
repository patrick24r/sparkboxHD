#ifndef INC_SPARKBOXLEVEL_HPP_
#define INC_SPARKBOXLEVEL_HPP_

#include <string>
#include "sparkboxErrors.hpp"
#include "SparkboxAudioManager.hpp"
#include "SparkboxVideoManager.hpp"

using namespace std;

class SparkboxLevel
{
public:
	SparkboxLevel();
	sparkboxError_t importLevel(string& levelDirectory);
	SparkboxAudioManager* audioMgr;
	SparkboxVideoManager* videoMgr;

};

#endif /* INC_SPARKBOXLEVEL_HPP_ */
