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

extern "C" void startupTask(void * argument)
{
  //string directoryName = "noDirectoryYet"; 
  /* Initialize USB - this needs to be done suring */
  MX_USB_HOST_Init();
  while (Appli_state != APPLICATION_READY) {
    osDelay(100);
  }
  
  //testFatfs();

  sparkboxLevelInit("noDirectoryYet");
  while(1) {
    osDelay(10);
  }
}