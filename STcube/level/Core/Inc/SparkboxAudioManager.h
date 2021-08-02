/*
 * SparkboxAudioManager.h
 *
 *  Created on: Jul 30, 2021
 *      Author: patri
 */

#ifndef INC_SPARKBOXAUDIOMANAGER_H_
#define INC_SPARKBOXAUDIOMANAGER_H_

#include "stm32h7xx.h"
#include <queue>

extern "C" void cb_dmaComplete(void);
extern "C" void cb_audioTimerIT(void);


using namespace std;

class SparkboxAudioManager
{
	SparkboxAudioManager(uint8_t audioStreams) {
		numberOfStreams = audioStreams;
	}
public:
	static void callback_AudioTimerIT(void);
	static void callback_DMATransferComplete(void);


private:
	uint8_t numberOfStreams;
	// List<int> audioStreams;
	Queue<> dmaRequestQueue;
};


#endif /* INC_SPARKBOXAUDIOMANAGER_H_ */
