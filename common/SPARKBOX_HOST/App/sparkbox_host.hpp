#ifndef INC_SPARKBOX_HOST_HPP_
#define INC_SPARKBOX_HOST_HPP_

// Global includes
#include "SparkboxLevel.hpp"

// Platform includes
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "cmsis_os2.h"
#include "gpio.h"
#include "fatfs.h"
#include "mdma.h"
#include "fmc.h"
#include "dac.h"
#include "tim.h"

extern SparkboxLevel* spkLevel;

sparkboxError_t SparkboxLevelInit(string& levelDirectory);

/* Audio Low Level Driver */
sparkboxError_t audioInitialize_lowLevel(void);
sparkboxError_t audioWriteSample_lowLevel(unsigned int newSample);
sparkboxError_t audioBeginDMATx_lowLevel(void* txFromAddress, void* txToAddress, unsigned int sizeBytes);
void audioDeinitialize_lowLevel(void);



#endif // !INC_SPARKBOX_HOST_HPP_
