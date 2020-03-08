#pragma once
#include "allPeripherals.h"
#include <vector>

#define I2C_EN0_Pin GPIO_PIN_8
#define I2C_EN0_GPIO_Port GPIOB
#define I2C_EN1_Pin GPIO_PIN_9
#define I2C_EN1_GPIO_Port GPIOB
#define I2C_EN2_Pin GPIO_PIN_0
#define I2C_EN2_GPIO_Port GPIOE
#define I2C_EN3_Pin GPIO_PIN_1
#define I2C_EN3_GPIO_Port GPIOE

// Sparkbox signature - if a controller can't return this
// the controller is deemed not connected
#define CONTROLLER_ADDRESS (0x0C)
/* I2C slave Register map:
 * LAX -> Left analog X
 * LAY -> Left analog Y
 * RAX -> Right analog X
 * RAY -> Right analog Y
 * ------------------------------------------------------------------------------------------------
 * |        |                                   Bit Number                                  | R/W |
 * ------------------------------------------------------------------------------------------------
 * | Reg    |    7    |    6    |    5    |    4    |    3    |    2    |    1    |    0    |     |
 * ------------------------------------------------------------------------------------------------
 * | 0x10   | LED[7]  | LED[6]  | LED[5]  | LED[4]  | LED[3]  | LED[2]  | LED[1]  | LED[0]  | R/W |
 * | 0x11   | BUT[7]  | BUT[6]  | BUT[5]  | BUT[4]  | BUT[3]  | BUT[2]  | BUT[1]  | BUT[0]  | R   |
 * | 0x12   | LAX[7]  | LAX[6]  | LAX[5]  | LAX[4]  | LAX[3]  | LAX[2]  | LAX[1]  | LAX[0]  | R   |
 * | 0x13   | LAY[3]  | LAY[2]  | LAY[1]  | LAY[0]  | LAX[11] | LAX[10] | LAX[9]  | LAX[8]  | R   |
 * | 0x14   | LAY[11] | LAY[10] | LAY[9]  | LAY[8]  | LAY[7]  | LAY[6]  | LAY[5]  | LAY[4]  | R   |
 * | 0x15   | RAX[7]  | RAX[6]  | RAX[5]  | RAX[4]  | RAX[3]  | RAX[2]  | RAX[1]  | RAX[0]  | R   |
 * | 0x16   | RAY[3]  | RAY[2]  | RAY[1]  | RAY[0]  | RAX[11] | RAX[10] | RAX[9]  | RAX[8]  | R   |
 * | 0x17   | RAY[11] | RAY[10] | RAY[9]  | RAY[8]  | RAY[7]  | RAY[6]  | RAY[5]  | RAY[4]  | R   |
 * ------------------------------------------------------------------------------------------------
 */
#define LED_POS (0U)
#define LED_MASK (0xFFUL << LED_POS)
#define BUTTON_POS (8U)
#define BUTTON_MASK (0xFFUL << BUTTON_POS)
#define LEFT_ANALOG_X_POS (16U) 
#define LEFT_ANALOG_X_MASK (0xFFFUL << LEFT_ANALOG_X_POS)
#define LEFT_ANALOG_Y_POS (28U) 
#define LEFT_ANALOG_Y_MASK (0xFFFUL << LEFT_ANALOG_Y_POS)
#define RIGHT_ANALOG_X_POS (40U) 
#define RIGHT_ANALOG_X_MASK (0xFFFUL << RIGHT_ANALOG_X_POS)
#define RIGHT_ANALOG_Y_POS (52U) 
#define RIGHT_ANALOG_Y_MASK (0xFFFUL << RIGHT_ANALOG_Y_POS)
// Maximum number of allowable controllers
#define MAX_CONTROLLERS 4

class SparkboxController
{
public:

  SparkboxController();
  ~SparkboxController();

  // Enumerations for building commands
  enum controllerSelect : uint8_t {
    CONTROLLER_0 = 0,
    CONTROLLER_1 = 1,
    CONTROLLER_2 = 2,
    CONTROLLER_3 = 3
  };

  // Update a single controller's fields
  int32_t syncSingleController(controllerSelect controller);
  // Update all connected controller fields
  int32_t syncAllControllers(void);
  // get the total number of connected controllers
  uint8_t getNumberConnected(void);
  // Test individual connection of controller
  int32_t isConnected(controllerSelect controller);

  // Set and get controller status
  uint8_t getLeds(controllerSelect controller);
  void setLeds(controllerSelect controller, uint8_t leds);
  uint8_t getPushButtons(controllerSelect controller);
  uint16_t getLeftAnalogX(controllerSelect controller);
  uint16_t getLeftAnalogY(controllerSelect controller);
  uint16_t getRightAnalogX(controllerSelect controller);
  uint16_t getRightAnalogY(controllerSelect controller);
  
    
private:
  // Enable one particular controller at a time
  void enableController(controllerSelect controller);

  // I2C handle
  I2C_HandleTypeDef hi2c1;
  // GPIO enable lines handle
  GPIO_InitTypeDef GPIO_InitStruct;

  // Bits specify which specific controllers are connected
  uint8_t connected; 

  /** Controller status data
   * Bits  | Function (Read/Write)
   * -----------------------------
   * 0-7   | LED status (W) 
   * 8-15  | Push button status (R)
   * 16-27 | Left analog stick X (R)
   * 28-39 | Left analog stick Y (R)
   * 40-51 | Right analog stick X (R)
   * 52-63 | Right analog stick Y (R)
   */ 
  std::vector<uint64_t> controllerData;

    
};