#ifndef INC_SPARKBOXCALLBACKS_H_
#define INC_SPARKBOXCALLBACKS_H_


#ifdef __cplusplus
#include "SparkboxLevel.h"
extern "C" {
#endif

void HAL_SDRAM_DMA_XferCpltCallback(MDMA_HandleTypeDef *hmdma);
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

#endif
