#include "SparkboxController.h"


SparkboxController::SparkboxController()
{
  // Reserve memory for each controller
  controllerData.reserve(MAX_CONTROLLERS);

  // Initialize GPIO pins for i2c enable
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, I2C_EN0_Pin|I2C_EN1_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, I2C_EN2_Pin|I2C_EN3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : I2C_EN0_Pin I2C_EN1_Pin */
  GPIO_InitStruct.Pin = I2C_EN0_Pin|I2C_EN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : I2C_EN2_Pin I2C_EN3_Pin */
  GPIO_InitStruct.Pin = I2C_EN2_Pin|I2C_EN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  // Initialize i2c
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c1);

  // Synchronize all controllers
  syncAllControllers();
}

// Synchronize a single controller
int32_t SparkboxController::syncSingleController(controllerSelect controller)
{
  uint8_t* startAddr; // i2c data address variable
  int32_t error;

  // Bounds check the controller input
  if (controller >= MAX_CONTROLLERS) return INVALID_CONTROLLER;

  // Set enable lines to only enable the correct controller
  enableController(controller);

  // Write LED status to controller, blocking
  startAddr = (uint8_t*)(controllerData.data + controller);
  // Timeout is 10 ms
  error = HAL_I2C_Master_Transmit(&hi2c, CONTROLLER_ADDRESS << 1, startAddr, 1, 10);
  if (error != HAL_OK) {
    connected &= ~(0x01 << controller);
    return error;
  }

  // Read button and analog stick data from controller, blocking
  startAddr = (uint8_t*)(controllerData.data + controller) + 1;
  // Timeout is 10 ms, return on error
  error = HAL_I2C_Master_Receive(&hi2c, CONTROLLER_ADDRESS << 1, startAddr, 7, 10);
  if (error != HAL_OK) {
    connected &= ~(0x01 << controller);
    return error;
  }

  // Indicate the controller is connected
  connected |= 0x01 << controller;
  return HAL_OK;
}

// Synchronize all controller data
int32_t SparkboxController::syncAllControllers()
{
  int32_t error = 0;

  for (controllerSelect i = 0; i < MAX_CONTROLLERS && !error; i++) {
    // Checks to see if a controller is connected
    error = syncSingleController(i);

    if (error && HAL_I2C_GetError(&hi2c) != HAL_I2C_ERROR_AF) break;
    else error = HAL_OK;
  }
  return error;
}

// Get the total number of connected controllers
uint8_t SparkboxController::getNumberConnected(void)
{
  uint8_t numberConnected = 0;

  for (uint8_t i = 0; i < MAX_CONTROLLERS; i++) {
    numberConnected += ((connected >> i) & 0x01);
  }

  return numberConnected;
}

// Check connection status of controllers
int32_t SparkboxController::isConnected(controllerSelect controller)
{
  return ((connected >> controller) & 0x01);
}

// Get the internal LED status
uint8_t SparkboxController::getLeds(controllerSelect controller)
{
  return (uint8_t)((controllerData.at(controller) & LED_MASK) 
          >> LED_POS);
}

// Set the controller LED values internally
void SparkboxController::setLeds(controllerSelect controller, uint8_t leds)
{
  controllerData.at(controller) &= ~(LED_MASK);
  controllerData.at(controller) |= (leds << LED_POS);
}

// Get the internal push button status
uint8_t SparkboxController::getPushButtons(controllerSelect controller)
{
  return (uint8_t)((controllerData.at(controller) & BUTTON_MASK) 
          >> BUTTON_POS);
}

// Get the internal left analog x status
uint16_t SparkboxController::getLeftAnalogX(controllerSelect controller)
{
  return (uint16_t)((controllerData.at(controller) & LEFT_ANALOG_X_MASK) 
          >> LEFT_ANALOG_X_POS);
}

// Get the internal left analog y status
uint16_t SparkboxController::getLeftAnalogY(controllerSelect controller)
{
  return (uint16_t)((controllerData.at(controller) & LEFT_ANALOG_Y_MASK) 
          >> LEFT_ANALOG_Y_POS);
}

// Get the internal right analog x status
uint16_t SparkboxController::getRightAnalogX(controllerSelect controller)
{
  return (uint16_t)((controllerData.at(controller) & RIGHT_ANALOG_X_MASK) 
          >> RIGHT_ANALOG_X_POS);
}

// Get the internal right analog y status
uint16_t SparkboxController::getRightAnalogY(controllerSelect controller)
{
  return (uint16_t)((controllerData.at(controller) & RIGHT_ANALOG_Y_MASK) 
          >> RIGHT_ANALOG_Y_POS);
}

// Enable one controller and disable the others
void SparkboxController::enableController(controllerSelect controller)
{
  uint8_t mask = 0x01 << controller;

  // Bounds check the controller
  if (controller >= MAX_CONTROLLERS) return;

  // Write to enable line pins - set only the enabeld pin
  switch (controller) {
    case 0: 
      HAL_GPIO_WritePin(GPIOB, I2C_EN1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOE, I2C_EN2_Pin|I2C_EN3_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB, I2C_EN0_Pin, GPIO_PIN_SET);
      break;
    case 1: 
      HAL_GPIO_WritePin(GPIOB, I2C_EN0_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOE, I2C_EN2_Pin|I2C_EN3_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB, I2C_EN1_Pin, GPIO_PIN_SET);
      break;
    case 2: 
      HAL_GPIO_WritePin(GPIOB, I2C_EN0_Pin|I2C_EN1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOE, I2C_EN3_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOE, I2C_EN2_Pin, GPIO_PIN_SET);
      break;
    case 3: 
      HAL_GPIO_WritePin(GPIOB, I2C_EN0_Pin|I2C_EN1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOE, I2C_EN2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOE, I2C_EN3_Pin, GPIO_PIN_SET);
      break;
    default: 
      HAL_GPIO_WritePin(GPIOB, I2C_EN0_Pin|I2C_EN1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOE, I2C_EN2_Pin|I2C_EN3_Pin, GPIO_PIN_RESET);
      break;
  }
  
}