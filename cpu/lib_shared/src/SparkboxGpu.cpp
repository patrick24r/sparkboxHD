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

  // Reset total bytes imported to 0
  totalImportedFileSizeBytes = 0;

  // Reserve space for MAX_LAYER_COUNT total layers
  layers.reserve(MAX_LAYER_COUNT);
  // and MAX_ACTIVE_LAYERS active ones
  activeLayers.reserve(MAX_ACTIVE_LAYERS);
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

int32_t Sparkbox::addAllSprites(std::string directory)
{
  // Find all files in the root directory

  // If file is a sprite file, import it
  // Call this function recursively if a sub-directory is found
}

// Import an individual sprite
int32_t Sparkbox::addSprite(std::string filePath)
{
  FRESULT error;
  FILINFO fileInfo;
  FIL file;
  uint8_t spriteDataBuffer[SPRITE_BUFFER_SIZE];
  uint32_t bytesRead;
  // Initialize new sprite and import the header data
  Layer newLayer = Layer(SPRITE, filePath);

  

  // If the width, height, and number of frames did not populate,
  // layer instantiation failed for the given file name
  if (newLayer.getWidth() == 0 || 
      newLayer.getHeight() == 0 || 
      newLayer.getNumberOfFrames() == 0) {
        // Add error code field to Layer class and return that instead?
        return ERR_LAYER_INST_FAILED;
  }

  // Verify that the sprite can be added to the list
  if (layers.size() == MAX_LAYER_COUNT) {
    return ERR_TOO_MANY_LAYERS;
  }

  // Verify file size by getting file statistics
  error = f_stat(filePath.c_str, &fileInfo);
  if (error) {
    return (int32_t)(error);
  }

  // If file size cannot be added to Sparkbox GPU, do not import it
  if (fileInfo.fsize + totalImportedFileSizeBytes > MAX_LEVEL_STORAGE) {
    return ERR_NOT_ENOUGH_MEMORY;
  }

  // Make sure file is not a directory
  if (fileInfo.fattrib & AM_DIR) {
    return ERR_NOT_A_FILE;
  }
  
  // Set the layerID to the position in the layers vector
  newLayer.setLayerID(layers.size());

  // Add imported sprite to layer vector
  importedLayers.push_back(newLayer);

  // Update the total file size
  totalImportedFileSizeBytes += (uint32_t)(fileInfo.fsize);

  // Open file to read sprite data
  error = f_open(&file, filePath.c_str, FA_READ);
  if (error) {
    return (int32_t)(error);
  }

  // Move sprite data from disk to the GPU in chunks
  do {
    // Read data from the file system 
    error = f_read(&file, spriteDataBuffer, SPRITE_BUFFER_SIZE, &bytesRead);
    // On file read error, 
    if (error) {
      f_close(&file);
      return (int32_t)(error);
    }

    // Write data to GPU RAM 
    writeRam(importedLayers.back().getLayerID(), spriteDataBuffer, bytesRead / 2);
    // If the bytes read != requested bytes, we hit the end of the file
  } while (bytesRead == SPRITE_BUFFER_SIZE);

  // DONE

}

// Add a text layer to the total layer
int32_t SparkboxGpu::addText(std::string textString)
{
  Layer newLayer = Layer(TEXT, textString);

  // Verify that the sprite can be added to the list
  if (layers.size() == MAX_LAYER_COUNT) {
    return ERR_TOO_MANY_LAYERS;
  }

  // Verify that the text size is not too large to be stored
  // Each character is 1 byte, size 
  if (textString.size() + totalImportedFileSizeBytes > MAX_LEVEL_STORAGE) {
    return ERR_NOT_ENOUGH_MEMORY;
  }

  // Set the layerID to the position in the layers vector
  newLayer.setLayerID(layers.size());

  // Add imported sprite to layer vector
  importedLayers.push_back(newLayer);

  // Update the total file size
  totalImportedFileSizeBytes += (uint32_t)(textString.size());

  // Write text data to GPU RAM
  writeRam(importedLayers.back().getLayerID(), textString.c_str(), textString.size() / 2);
}

// Intelligently synchronize layer header data between GPU and CPU
void SparkboxGpu::synchronizeActiveLayers(void)
{
  // Read layer header data from GPU that may have been changed
  smartUpdateLayerHeadersFromGpu();

  // Write back all layer headers
  updateLayerHeadersFromCpu();

  // Write palette data for each layer to the GPU
  updateLayerPalettesFromCpu();
}

// Write all active layer data to the GPU, using active layer
// data as the source of truth
void SparkboxGpu::writeActiveLayers(void)
{
  // Write layer headers
  updateLayerHeadersFromCpu();

  // Write palette data for each layer
  updateLayerPalettesFromCpu();
}

// Update fields from GPU, but only some fields
// Only update fields the GPU may change internally
void SparkboxGpu::smartUpdateLayerHeadersFromGpu(void)
{
  uint16_t gpuReadData;
  Layer iterLayer;

  // Loop over every layer to look for updates
  for (uint8_t i = 0; i < activeLayers.size(); i++) {
    iterLayer = activeLayers.at(i);

    // GPU will not internally change text data, go to next layer
    if (iterLayer.getLayerType() == TEXT) continue;

    // Update the current frame number (in the case the sprite is animated)
    gpuReadData = readLayerHeaders(i,7) >> 8;
    iterLayer.setCurrentFrameNumber((uint8_t)gpuReadData);

    // Update the current x position (in the case xVelocity is nonzero)
    gpuReadData = readLayerHeaders(i, 3);
    iterLayer.setxPosition((int16_t)gpuReadData);

    // Update the current y position (in the case yVelocity is nonzero)
    gpuReadData = readLayerHeaders(i, 4);
    iterLayer.setyPosition((int16_t)gpuReadData);

    // Save changed layer back in same position
    activeLayers.at(i) = iterLayer;
  }
}

// Update all layer header fields from GPU,
// using GPU as source of truth
void SparkboxGpu::updateLayerHeadersFromGpu(void)
{
  uint16_t gpuReadData;
  Layer iterLayer;

  for (uint8_t i = 0; i < activeLayers.size(); i++) { // Loop over each active layer
    for (uint8_t j = 0; j < 8; j++) { // Loop over each layer header register
      iterLayer = activeLayers.at(i);
      gpuReadData = readLayerHeaders(i, j);

      switch (j) {
      case 0 : // Layer headers
        // Check status of animated and visible flags
        iterLayer.setVisible((gpuReadData >> 2) & 0x0001);
        iterLayer.setAnimated((gpuReadData >> 3) & 0x0001);
        break;

      case 1 : // Width
        iterLayer.setWidth(gpuReadData);
        break;

      case 2 : // Height
        iterLayer.setHeight(gpuReadData);
        break;

      case 3 : // X Position
        iterLayer.setxPosition((int16_t)gpuReadData);
        break;

      case 4 : // Y Position
        iterLayer.setyPosition((int16_t)gpuReadData);
        break;

      case 5 : // Sprite - x velocity, Text - font selection
        iterLayer.setxVelocity((int16_t)gpuReadData);
        iterLayer.setFont(gpuReadData);
        break;

      case 6 : // Sprite - y velocity, Text - number of visible chars
        iterLayer.setyVelocity((int16_t)gpuReadData);
        iterLayer.setNumberOfVisibleCharacters(gpuReadData);
        break;

      case 7 : // Sprite - number of frames / current frame, Text - palette index
        iterLayer.setCurrentFrameNumber((uint8_t)gpuReadData);
        iterLayer.setFontPaletteSelection((uint8_t)gpuReadData);
        break;

      default :
        break;
      }

      activeLayers.at(i) = iterLayer;
    }
  }
}

// Write all layer headers in CPU to GPU
void SparkboxGpu::updateLayerHeadersFromCpu(void)
{
  uint16_t regValue;

  for (uint8_t i = 0; i < activeLayers.size(); i++) { // Loop over each active layer
    for (uint8_t j = 0; j < 8; j++) { // Loop over each layer header register
      // Get the value of the layer register at index j based on layer data
      regValue = activelayers.at(i).getLayerRegisterValue(j);
      // Write the layer header to the GPU
      writeLayerHeaders(i, j, regValue);
    }
  }
}

// Overwrite palette data in GPU with palette data in this class
void SparkboxGpu::updateLayerPalettesFromCpu(void)
{
  for (uint8_t i = 0; i < activeLayers.size(); i++) { // Loop over active layers
    for (uint j = 0; j < PALETTE_SIZE; j++) { // Loop over palette slots
      // Args: layer, palette slot, color data
      writePalette(i, j, activeLayers.at(i).getPaletteAt(j));
    }
  }
}


/* ----------------PRIVATE METHODS --------------------- */

// This function is here so the user CAN send raw commands
// but is heavily discouraged
uint16_t SparkboxGpu::sendRawCommand(uint16_t command, uint16_t data)
{
  return sendGpuCommand(command, data);
}

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

  // Set chip select pin for GPU
  // Disable GPU output
  // Set CPU data pins to output

  // Set clk pin to low
  // Set command pins to command
  // Set data pins to data
  // Set clk pin to high

  // Pause for ~20 us (Max time for command reads to process)

  // Set data pins to inputs
  // Enable GPU output
  // Set clk pin to low
  // Get data from data pins

  // Return data
}