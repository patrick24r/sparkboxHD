void SparkboxGpu::SparkboxGpu(void)
{
  // Reset GPU when instantiating
  resetGpu();
}

static uint16_t SparkboxGpu::sendGpuCommand(uint16_t command, uint16_t data)
{
  // Set chip select pin for GPU
  // Set chip to write mode
  // Set data pins to output

  // Set clk pin to low
  // Set command pins to command
  // Set data pins to data
  // Set clk pin to high

  // Pause for ~20 us (Max time for command reads to process)

  // Set data pins to inputs
  // Set chip to read mode
  // Set clk pin to low
  // Get data from data pins

  // Return data
}

static uint16_t SparkboxGpu::generateGpuCommand(commandType type, commandTarget target)
{
    return generateGpuCommand(commandType type, commandTarget target, 0);
}

static uint16_t SparkboxGpu::generateGpuCommand(commandType type, commandTarget target, uint16_t parameters)
{
    // returnVal[15:14] = type[1:0]
    // returnVal[13:11] = target[2:0]
    // returnVal[10:0] = parameters[10:0]
    return ((type & 0x03) << 14) | ((target & 0x07) << 11) | (parameters & 0x03FF);
}

static void SparkboxGpu::resetGpu(void)
{
  sendGpuCommand(generateGpuCommand(GPU_RESET, GPU_ALL, GPU));
}
