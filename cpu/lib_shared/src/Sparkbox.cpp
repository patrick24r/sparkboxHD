#include "Sparkbox.h"

Sparkbox::Sparkbox(std::string *levelDirectory)
{
  // Save the file path to the root directory of the current level
  rootDirectory = *levelDirectory;
  // Reset total bytes imported to 0
  totalImportedFileSizeBytes = 0;

  // TODO: Have input arguments define pins maybe?
  // That would make this WAY more flexible
  gpu = new SparkboxGpu();
  audio = new SparkboxAudio();
  controller = new SparkboxController();

  // Reserve space for MAX_LAYER_COUNT total layers
  layers.reserve(MAX_LAYER_COUNT);
  // and MAX_ACTIVE_LAYERS active ones
  activeLayers.reserve(MAX_ACTIVE_LAYERS);

}

// Delete all Sparkbox handles
Sparkbox::~Sparkbox()
{
  delete gpu;
  delete audio;
  delete controller;
}


errorCode_t Sparkbox::importSprite(std::string filePath)
{
  FRESULT error;
  FILINFO fileInfo;
  FIL file;
  uint8_t spriteDataBuffer[SPRITE_BUFFER_SIZE];
  uint32_t bytesRead;
  Layer newLayer = Layer(SPRITE, filePath.c_str);

  // Verify that the sprite can be added to the list
  if (layers.size() == MAX_LAYER_COUNT) return ERR_TOO_MANY_LAYERS;

  // If the width, height, and number of frames did not populate,
  // layer instantiation failed for the given file name
  if (newLayer.getWidth() == 0 || 
      newLayer.getHeight() == 0 || 
      newLayer.getNumberOfFrames() == 0) {
        return ERR_LAYER_INST_FAILED;
  }

  // Verify file size by getting file statistics
  error = f_stat(filePath.c_str, &fileInfo);
  if (error) return (errorCode_t)(error);
  // If file size 
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
  layers.push_back(newLayer);
  // Update the total file size
  totalImportedFileSizeBytes += (uint32_t)(fileInfo.fsize);

  // Open file to read sprite data
  error = f_open(&file, filePath.c_str, FA_READ);
  if (error) return (errorCode_t)(error);
  // Move sprite data from disk to the GPU in chunks
  do {
    // Read data from the file system 
    error = f_read(&file, spriteDataBuffer, SPRITE_BUFFER_SIZE, &bytesRead);
    // On file read error, 
    if (error) {
      f_close(&file);
      return (errorCode_t)(error);
    }
    // Write data to GPU RAM 
    gpu->writeRam(layers.back().getLayerID(), spriteDataBuffer, bytesRead / 2);
    // If the bytes read != requested bytes, we hit the end of the file
  } while (bytesRead == SPRITE_BUFFER_SIZE);

  // DONE

}

errorCode_t Sparkbox::importLevel(void)
{
  // Find all files in the root directory

  // Call this function recursively if a sub-directory is found

}