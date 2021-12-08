#include "sparkboxCallbacks.h"
#include "sparkboxMain.h"

/* Audio Callbacks */
void sparkboxCallback_audioDMACplt(MDMA_HandleTypeDef *hmdma)
{
	if (hmdma->Instance == MDMA_Channel0) {
		// Call audio dma done callback
		if (spark != NULL && spark->audioMgr != NULL) spark->audioMgr->callback_DMATransferComplete();
	}
	return;
}

void sparkboxCallback_audioIT(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM7) {
		// Call audio timer trigger callback
		if (spark != NULL && spark->audioMgr != NULL) spark->audioMgr->callback_AudioTimerIT();
	}
	return;
}
