#ifndef INC_SPARKBOXCALLBACKS_H_
#define INC_SPARKBOXCALLBACKS_H_

#ifdef __cplusplus

extern "C" {
#endif

#include "mdma.h"
#include "tim.h"

// Audio interrupt callbacks
void sparkboxCallback_audioDMACplt(MDMA_HandleTypeDef *hmdma);
void sparkboxCallback_audioIT(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

#endif
