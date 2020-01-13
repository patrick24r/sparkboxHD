#include "SparkboxGpu.h"

void SparkboxGpu::SparkboxGpu(void)
{
  // Initialize all pins
  // Command pins as outputs
  // Data pins as inputs for now
  // RDY/#BSY as input
  // Command clock as output
  // Output enable as output
  // Chip select as output

  // Reset GPU when instantiating
  resetAll();

  
}

// Reset all layer headers
void SparkboxGpu::resetLayerHeaders(void)
{
  // Send command to reset all layer headers
  sendGpuCommand(TYPE_RESET | TARGET_LAYERHEADERS | ALL_LAYERS);
}

// Reset a particular layer's header
void SparkboxGpu::resetLayerHeaders(uint8_t layer)
{
  sendGpuCommand(TYPE_RESET | TARGET_LAYERHEADERS | layer);
}

// Read from the layer headers
uint16_t SparkboxGpu::readLayerHeaders(uint8_t layer, uint8_t registerIndex)
{
  // Bounds check inputs (unsigned, no need for lower bounds check)
  if (registerIndex > 7) registerIndex = 7;
  if (layer > 31) layer = 31;

  // Read desired data from the GPU
  return sendGpuCommand(TYPE_READ | TARGET_LAYERHEADERS | 
         (registerIndex << 6) | layer);
}

// Write to the layer headers
void SparkboxGpu::writeLayerHeaders(uint8_t layer, uint8_t registerIndex, uint16_t data)
{
  if (registerIndex > 7) registerIndex = 7;
  if (layer > 31) layer = 31;

  sendGpuCommand(TYPE_WRITE | TARGET_LAYERHEADERS | (registerIndex << 6) | layer, data);
}

// Reset the GPU RAM - all layer specific raw data
void SparkboxGpu::resetRam(void)
{
  sendGpuCommand(TYPE_RESET | TARGET_RAM);
}

// Read layer data from RAM
// layerID - layerID associated with the layer to read
// *dataToRead - pointer to the data buffer to store read data
// size - number of 16 bit reads to perform
void SparkboxGpu::readRam(uint8_t layerID, uint16_t *dataToRead, uint16_t size)
{
  // Successive reads from the same layerID will return successive data from RAM
  for (uint16_t i = 0; i < size; i++){
    dataToRead[i] = sendGpuCommand(TYPE_READ | TARGET_RAM | (layerID << 3));
  }

}

// Write layer data to RAM
// layerID - layerID associated with the layer to write
// *dataToWrite - pointer to the data buffer to store write data
// size - number of 16 bit writes to perform
void SparkboxGpu::writeRam(uint8_t layerID, uint16_t *dataToWrite, uint16_t size)
{
  // Successive writes to the same layerID will write to successive data locations in RAM
  for (uint16_t i = 0; i < size; i++){
    sendGpuCommand(TYPE_WRTIE | TARGET_RAM | (layerID << 3), dataToWrite[i]);
  }
}

// Reset Flash memory
// WARNING: UNTIL NEW TEXT DATA IS WRITTEN TO FLASH, FONT ENGINE WILL NOT WORK
void SparkboxGpu::resetFlash(void)
{
  sendGpuCommand(TYPE_RESET | TARGET_FLASH);
}

// Read Flash memory
// fontIndex - index of the font to read
// *dataToWrite - pointer to the data buffer to store write data
// size - number of 16 bit reads to perform
void SparkboxGpu::readFlash(uint16_t fontIndex, uint16_t *dataToRead, uint16_t size)
{  
  // Font index is 11 bits, clear bits [15:11] here
  fontIndex &= ~(0xF800);

  for(uint16_t i = 0; i < size; i++){
    dataToRead[i] = sendGpuCommand(TYPE_READ | TARGET_FLASH | fontIndex);
  }
}

// Write Flash memory
// fontIndex - index of the font to write
// *dataToWrite - pointer to the data buffer with write data
// size - number of 16 bit writes to perform
void SparkboxGpu::writeFlash(uint16_t fontIndex, uint16_t *dataToWrite, uint16_t size)
{
  // Font index is 11 bits, clear bits [15:11] here
  fontIndex &= ~(0xF800);

  for(uint16_t i = 0; i < size; i++){
    sendGpuCommand(TYPE_WRITE | TARGET_FLASH | fontIndex, dataToWrite[i]);
  }
}

// Reset the palette memory
void SparkboxGpu::resetPalette(void)
{
  sendGpuCommand(TYPE_RESET | TARGET_PALETTE);
}

// Read an individual palette slot
uint32_t SparkboxGpu::readPalette(uint8_t layer, uint8_t colorSlot)
{
  uint16_t colorRG, colorB;

  // Read the RG color slot
  colorRG = sendGpuCommand(TYPE_READ | TARGET_PALETTE | 
    ((colorSlot & 0x1F) << 6) | // Only bits 4:0 are valid 
    1 << 5 | // RG specifier
    (layer & 0x1F) // Only bits 4:0 are valid
  );

  // Read the RG color slot
  colorB = sendGpuCommand(TYPE_READ | TARGET_PALETTE | 
    ((colorSlot & 0x1F) << 6) | // Only bits 4:0 are valid 
    0 << 5 | // B specifier
    (layer & 0x1F) // Only bits 4:0 are valid
  );

  return ((colorRG << 16) | colorB);
}

// Write to an individual palette slot
void SparkboxGpu::writePalette(uint8_t layer, uint8_t colorSlot, uint32_t color)
{
  // Read the RG color slot
  sendGpuCommand(TYPE_WRITE | TARGET_PALETTE | 
    ((colorSlot & 0x1F) << 6) | // Only bits 4:0 are valid 
    1 << 5 | // RG specifier
    (layer & 0x1F), // Only bits 4:0 are valid
    (uint16_t)(color >> 16); // Use bits 31:16 for Red & Green
  );

  // Read the RG color slot
  sendGpuCommand(TYPE_WRITE | TARGET_PALETTE | 
    ((colorSlot & 0x1F) << 6) | // Only bits 4:0 are valid 
    0 << 5 | // B specifier
    (layer & 0x1F), // Only bits 4:0 are valid
    (uint16_t)(color & 0xFFFF); // Use bits 15:0 for Blue
  );
}

// Reset all memories on the GPU (except Flash)
void SparkboxGpu::resetAll(void)
{
  sendGpuCommand(TYPE_RESET | TARGET_ALL, 0);
}

// Send command to begin rendering a new frame
void SparkboxGpu::startFrameUpdate(void)
{
  // Wait until GPU is ready
  waitUntilFree();
  sendGpuCommand(TYPE_SPECIAL | FRAME_UPDATE);
}

// Poll the RDY/#BSY GPU pin
uint8_t SparkboxGpu::isBusy(void)
{
  return (!RDY_PIN);
}

// Use polling to wait until GPU is free
void SparkboxGpu::waitUntilFree(void)
{
  // Wait until GPU is no longer busy
  while (isBusy());
}

// This function is here so the user CAN send raw commands
// but is heavily discouraged
uint16_t SparkboxGpu::sendRawCommand(uint16_t command, uint16_t data)
{
  return sendGpuCommand(command, data);
}

/* ----------------PRIVATE METHODS --------------------- */
// Helper function for sending a command
// This is used for reset/read commands where no write data is necessary
uint16_t SparkboxGpu::sendGpuCommand(uint16_t command)
{
  return sendGpuCommand(command, 0);
}

// Send a command to the GPU and return its response
uint16_t SparkboxGpu::sendGpuCommand(uint16_t command, uint16_t data)
{
  // Wait for GPU to no longer be busy

  // Set clk pin to low
  // Set chip select pin to high
  // Set data line pins to inputs
  // Enable GPU output
  // Set clk pin to high
  // Pause ~10 us
  // Set clk pin to low

  // Pause ~10 us

  // Disable GPU output
  // Set data pins to command
  // Set clk pin to high
  // Pause ~10 us
  // Set clk pin to low
  // Pause ~10 us
  // Set data line pins to data
  // Set clk pin to high
  // Pause ~10 us
  // Set clk pin to low

  // Pause for ~20 us (Max time for command reads to process)

  // Set data line pins to inputs
  // Enable GPU output
  // Pause ~10 us
  // Set clk pin to high
  // Pause ~10 us
  // Get data from data pins
  // Set clk pin to low

  // Return data
}