#include "SparkboxController.h"


SparkboxController::SparkboxController()
{
  // Reserve memory for each controller
  controllerData.reserve(MAX_CONTROLLERS);

  // Initialize i2c, see HAL_I2C_MspInit() function for setup details
  HAL_I2C_Init(&hi2c);
  HAL_GPIO_Init(GPIOB, &gpioEn)

  // Synchronize all controllers
  error = syncAllControllers();
}

// Synchronize a single controller
int32_t SparkboxController::syncSingleController(controllerSelect controller)
{
  uint8_t* startAddr; // DMA start address

  // If under error, do nothing
  if (error) return error;

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

  // Write to enable line pins
}