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
sparkboxError_t sparkboxLevelInit(string levelDirectory);

#endif // !INC_SPARKBOX_HOST_HPP_
