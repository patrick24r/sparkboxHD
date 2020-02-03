#include "SparkboxVideo.h"

void SparkboxVideo::SparkboxVideo()
{
  gpu = SparkboxGpu();

  // Reset total bytes imported to 0
  totalImportedFileSizeBytes = 0;

  // Reserve space for MAX_LAYER_COUNT total layers
  layerBank.reserve(MAX_LAYER_COUNT);
  // and MAX_ACTIVE_LAYERS active ones
  activeLayers.reserve(MAX_ACTIVE_LAYERS);
}

uint8_t SparkboxVideo::loadAllSprites(std::string directory)
{
  // Find all files in the root directory

  // Call this function recursively if a sub-directory is found

  // If file is a sprite file, import it
  addSprite(directory);
  
}

/*!
 * Import an individual sprite from disk into CPU and GPU
 * @param File path of he sprite file on disk
 * @retval The layer ID of the just added sprite. Also the index of the
 *         just added sprite in the layer bank
 * TODO : Map fatfs error codes to negative numbers so successful return 
 * values can be the layer ID number of the just-added sprite
 */ 
int32_t SparkboxVideo::addSprite(std::string filePath)
{
  FRESULT error;
  FILINFO fileInfo;
  FIL file;
  uint8_t spriteDataBuffer[SPRITE_BUFFER_SIZE];
  uint32_t bytesRead;
  
  // Initialize new sprite
  Layer newLayer = Layer(SPRITE, filePath);

  // Check if the layer bank is already full
  if (layerBank.size() >= MAX_LAYER_COUNT) {
    return ERR_TOO_MANY_LAYERS;
  }

  // Verify that the file exists
  error = f_stat(filePath.c_str, &fileInfo);
  if (error) {
    return (int32_t)(error);
  }

  // Make sure file is not a directory
  if (fileInfo.fattrib & AM_DIR) {
    return ERR_NOT_A_FILE;
  }

  // If file size cannot be added to Sparkbox GPU, do not import it
  if (fileInfo.fsize - SPRITE_HEADER_BYTES + totalImportedFileSizeBytes > MAX_LEVEL_STORAGE) {
    return ERR_NOT_ENOUGH_GPU_MEMORY;
  }

  // Verify file size is larger than just the header
  if (fileInfo.fsize <= SPRITE_HEADER_BYTES) {
    return ERR_INVALID_SPRITE_FILE;
  }

  // Read from disk for sprite data
  // Import sprite header data from fatfs file system
  error = f_open(&file, filePath.c_str, FA_READ);
  if (error) {
    // Error opening the file
    return 0-(int32_t)(error);
  }

  // Read sprite header - includes palette, number of frames, width, height
  error = f_read(&file, textRead, SPRITE_HEADER_BYTES, &bytesRead);
  if (error) {
    // Didn't read the sprite header correctly
    f_close(&file);
    return 0-(int32_t)(error);
  }
  if (bytesRead != SPRITE_HEADER_BYTES) {
    return VIDEO_SPRITE_HEADER_READ;
  }

  // This is the only place these properties should be touched
  newLayer.setHeight((uint16_t)(textRead[] | (textRead[] << 8)));
  newLayer.setWidth((uint16_t)(textRead[] | (textRead[] << 8)));
  newLayer.setNumberOfFrames((uint8_t)(textRead[]));
  // Set palette colors to data read from file
  newLayer.setPalette((uint32_t*)(textRead + PALETTE_OFFSET_CHAR), PALETTE_SIZE);

  // Move sprite data from disk to the GPU in chunks
  do {
    // Read data from the file system 
    error = f_read(&file, spriteDataBuffer, SPRITE_BUFFER_SIZE, &bytesRead);

    // On file read error, close file and return error
    if (error) {
      f_close(&file);
      return 0-(int32_t)(error);
    }

    // Write data to GPU RAM 
    gpu.writeRam(layerBank.back().getLayerID(), spriteDataBuffer, bytesRead / 2);

    // If the bytes read != requested bytes, we hit the end of the file
  } while (bytesRead == SPRITE_BUFFER_SIZE);

  f_close(&file);

  // Set the layerID to the position in the layers vector
  newLayer.setLayerID(layerBank.size());

  // Add imported sprite to layer vector
  layerBank.push_back(newLayer);

  // Update the total file size
  totalImportedFileSizeBytes += (uint32_t)(fileInfo.fsize);

  // DONE, return the layer ID / index of final 
  return layerBank.back().getLayerID();
}

// Add a text layer to the total layer
int32_t SparkboxVideo::addText(std::string textString)
{
  Layer newLayer = Layer(TEXT, textString);

  // Verify that the sprite can be added to the list
  if (layers.size() >= MAX_LAYER_COUNT) {
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
  layerBank.push_back(newLayer);

  // Update the total file size
  totalImportedFileSizeBytes += (uint32_t)(textString.size());

  // Write text data to GPU RAM
  gpu.writeRam(layerBank.back().getLayerID(), textString.c_str(), textString.size() / 2);
}

// Retrieve layer data at a given layer bank location
Layer SparkboxVideo::getLayerAt(uint8_t layerID)
{
  return layerBank.at(layerID);
}

// Synchronize all video information between CPU and GPU
void SparkboxVideo::syncAllVideo(void)
{
  // Read layer header data from GPU that may have been changed
  smartUpdateLayerHeadersFromGpu();

  // Write back all active layers
  writeActiveLayers();
}

// Write all active layer data to the GPU, using active layer
// data as the source of truth
void SparkboxVideo::writeActiveLayers(void)
{
  // Write layer headers
  updateLayerHeadersFromCpu();

  // Write palette data for each layer
  updateLayerPalettesFromCpu();
}

// Update fields from GPU, but only some fields
// Only update fields the GPU may change internally
void SparkboxVideo::smartUpdateLayerHeadersFromGpu(void)
{
  uint16_t gpuReadData;
  Layer iterLayer;

  // Loop over every layer to look for updates
  for (uint8_t i = 0; i < activeLayers.size(); i++) {
    iterLayer = activeLayers.at(i);

    // GPU will not internally change text data, go to next layer
    if (iterLayer.getLayerType() == TEXT) continue;

    // Update the current frame number (in the case the sprite is animated)
    gpuReadData = gpu.readLayerHeaders(i,7) >> 8;
    iterLayer.setCurrentFrameNumber((uint8_t)gpuReadData);

    // Update the current x position (in the case xVelocity is nonzero)
    gpuReadData = gpu.readLayerHeaders(i, 3);
    iterLayer.setxPosition((int16_t)gpuReadData);

    // Update the current y position (in the case yVelocity is nonzero)
    gpuReadData = gpu.readLayerHeaders(i, 4);
    iterLayer.setyPosition((int16_t)gpuReadData);

    // Save changed layer back in same position
    activeLayers.at(i) = iterLayer;
  }
}

// Update all layer header fields from GPU,
// using GPU as source of truth
void SparkboxVideo::updateLayerHeadersFromGpu(void)
{
  uint16_t gpuReadData;
  Layer iterLayer;

  for (uint8_t i = 0; i < activeLayers.size(); i++) { // Loop over each active layer

    // Load the current saved layer
    iterLayer = activeLayers.at(i);

    for (uint8_t j = 0; j < 8; j++) { // Loop over each layer header register
      
      gpuReadData = gpu.readLayerHeaders(i, j);

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
    }

    // Write back the changed layer
    activeLayers.at(i) = iterLayer;
  }
}

// Write all layer headers in CPU to GPU
void SparkboxVideo::updateLayerHeadersFromCpu(void)
{
  uint16_t regValue;

  for (uint8_t i = 0; i < activeLayers.size(); i++) { // Loop over each active layer
    for (uint8_t j = 0; j < 8; j++) { // Loop over each layer header register
      // Get the value of the layer register at index j based on layer data
      regValue = activelayers.at(i).getLayerRegisterValue(j);
      // Write the layer header to the GPU
      gpu.writeLayerHeaders(i, j, regValue);
    }
  }
}

// Overwrite palette data in GPU with palette data in this class
void SparkboxVideo::updateLayerPalettesFromCpu(void)
{
  for (uint8_t i = 0; i < activeLayers.size(); i++) { // Loop over active layers
    for (uint j = 0; j < PALETTE_SIZE; j++) { // Loop over palette slots
      // Args: layer, palette slot, color data
      gpu.writePalette(i, j, activeLayers.at(i).getPaletteAt(j));
    }
  }
}

// Update the current frame displayed on the screen
void SparkboxVideo::updateFrame(void)
{
  gpu.startFrameUpdate();
  syncAllVideo();
}
