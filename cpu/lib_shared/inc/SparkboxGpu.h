// This class contains low level functions to interface
// with the Sparkbox GPU
#include "PeripheralHandles.h"
#include "Sprite.h"

class SparkboxGpu
{
public:
  // Constructor and Destructor
  SparkboxGpu();
  // Allow this function to be called without instantiating
  static uint16_t generateGpuCommand(GPU_COMMAND_TYPE type, GPU_COMMAND_TARGET target, uint16_t parameters);

  static uint16_t generateGpuCommand((GPU_COMMAND_TYPE type, GPU_COMMAND_TARGET target);

  static 
private:
  uint16_t sendGpuCommand(uint16_t command, uint16_t data);
}