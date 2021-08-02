#include "SparkboxAudioManager.h"

/* Wrapper functions for callback functions */
void cb_dmaComplete(void)
{
	SparkboxAudioManager::callback_DMATransferComplete();
}

void cb_audioTimerIT(void)
{
	SparkboxAudioManager::callback_AudioTimerIT();
}

static void SparkboxAudioManager::callback_DMATransferComplete(void)
{
	// Check DMA queue for any requests
}

static void SparkboxAudioManager::callback_AudioTimerIT(void)
{
	//
}


