/*
 * @file           : sparkboxStartup.cpp
 * @brief          : Main program body
 */
#include "sparkboxStartup.hpp"
#include "main.h"
#include "fatfs.h"
#include "usb_host.h"

extern volatile ApplicationTypeDef Appli_state;

void testFatfs();
void testSram();

extern "C" void startupTask(void * argument)
{

  // blink LEDs
  while(1)
  {
    HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
    osDelay(50);
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    osDelay(50);
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    osDelay(50);
    HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
    osDelay(50);
    HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
    osDelay(50);
    HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
    osDelay(50);
    HAL_GPIO_TogglePin(LED6_GPIO_Port, LED6_Pin);
    osDelay(50);
    HAL_GPIO_TogglePin(LED7_GPIO_Port, LED7_Pin);
    osDelay(50);
  }


  //string directoryName = "noDirectoryYet"; 
  /* Initialize USB - this needs to be done suring */
  MX_USB_HOST_Init();
  while (Appli_state != APPLICATION_READY) {
    osDelay(100);
  }

  //testFatfs();

  sparkboxLevelInit("game1");
  while(1) {
    HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
    osDelay(500);
  }
}


void testSram()
{

}

void testFatfs()
{
  unsigned int bw;
  while (Appli_state != APPLICATION_READY);
  HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
  retUSBH = f_open(&USBHFile, "testFile.txt", FA_CREATE_ALWAYS | FA_WRITE);
  if (retUSBH == FR_OK) {
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

    retUSBH = f_write(&USBHFile, "Hello", 5, &bw);
    if (retUSBH == FR_OK) {
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    }
    f_close(&USBHFile);

    while(1) osDelay(1);
  }
}