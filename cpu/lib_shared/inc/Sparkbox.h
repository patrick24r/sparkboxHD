#pragma once
#include <vector>
#include <string>
#include "allPeripherals.h"
#include "SparkboxError.h"
#include "SparkboxVideo.h"
#include "SparkboxAudio.h"
#include "SparkboxController.h"

#define LED0_Pin GPIO_PIN_12
#define LED0_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_14
#define LED2_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_15
#define LED3_GPIO_Port GPIOB
#define SDIO_Detect_Pin GPIO_PIN_13
#define SDIO_Detect_GPIO_Port GPIOD

// This class is a container for all Sparkbox peripherals
class Sparkbox 
{
public:
  Sparkbox();
  ~Sparkbox();

  // Import all level data on disk for the current level
  importLevel(void);

  // Return a random 32 bit number
  uint32_t rand(void);

  /* Sparkbox Handles */
  // Sparkbox GPU interface handle
  SparkboxGpu gpu;
  // Sparkbox audio interface handle
  SparkboxAudio audio;
  // Sparkbox controller interface handle
  SparkboxController controller;
  
private:
  // Handles only used by the sparkbox s
  SD_HandleTypeDef hsd;
  NAND_HandleTypeDef hnand1;
  RNG_HandleTypeDef hrng;
  TIM_HandleTypeDef htim13;

  /* Sparkbox private methods */
  lowLevelInit(void);
  lowLevelDeInit(void);

  /* Sparkbox private properties */
  // Root directory of the current level
  std::string rootLevelDirectory;
};