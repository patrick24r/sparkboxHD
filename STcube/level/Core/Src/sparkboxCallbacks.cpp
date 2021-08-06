#include "sparkboxCallbacks.h"

/* Audio Callbacks */
void HAL_SDRAM_DMA_XferCpltCallback(MDMA_HandleTypeDef *hmdma)
{
	return;
}

void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6)
	{
		// Call audio callback
	}
	return;
}
