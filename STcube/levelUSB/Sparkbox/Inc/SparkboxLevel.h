#ifndef INC_SPARKBOXLEVEL_H_
#define INC_SPARKBOXLEVEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"

#ifdef __cplusplus
}
#include "SparkboxAudioManager.h"
#include <string>

using namespace std;

class SparkboxLevel
{
public:
	SparkboxLevel(string levelFolder);
	SparkboxAudioManager* audioMgr;
	vector<uint8_t> audioFileIDs;

	static void audioThreadWrapper(void);

private:
	osThreadId_t audioManagerHandle;
	osThreadAttr_t audioManagerTask_attributes;
};

#endif /* __cplusplus */

#endif /* INC_SPARKBOXLEVEL_H_ */
