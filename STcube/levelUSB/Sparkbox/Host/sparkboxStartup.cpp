/*
 * @file           : consoleStartup.cpp
 * @brief          : Main program body
 */
#include "sparkboxStartup.hpp"
#include "main.h"
#include "fatfs.h"
#include "usb_host.h"

extern volatile ApplicationTypeDef Appli_state;

void testFatfs();
void testSdram();

extern "C" void levelStartupTask(void * argument)
{
  //string directoryName = "noDirectoryYet"; 
  /* Initialize USB - this needs to be done during */
  MX_USB_HOST_Init();
  /*while (Appli_state != APPLICATION_READY) {
    osDelay(100);
  }*/
  //testFatfs();
  testSdram();

  sparkboxLevelInit("noDirectoryYet");
  while(1) {
    HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    osDelay(500);
  }
}

void testFatfs()
{
  unsigned int bw;
  while (Appli_state != APPLICATION_READY);
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
  retUSBH = f_open(&USBHFile, "testFile.txt", FA_CREATE_ALWAYS | FA_WRITE);
  if (retUSBH == FR_OK) {
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

    retUSBH = f_write(&USBHFile, "Hello", 5, &bw);
    if (retUSBH == FR_OK) {
      HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
    }
    f_close(&USBHFile);

    while(1) osDelay(1);
  }
}

#define START_ADDR (0x70000000)
void testSdram()
{
  HAL_StatusTypeDef status;
  uint8_t testBuffer[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  HAL_SDRAM_WriteProtection_Disable(&hsdram1);

  status = HAL_SDRAM_Write_8b(&hsdram1, (uint32_t*)(START_ADDR + 0x0800), testBuffer, 8);

  while(1) osDelay(1);
}
