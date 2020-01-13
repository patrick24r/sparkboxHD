#pragma once
#include "allPeripherals.h"
#include "SparkboxGpu.h"
#include "Layer.h"

#define MAX_LAYER_COUNT 256
#define MAX_ACTIVE_LAYERS 32
#define SPRITE_BUFFER_SIZE 2048
#define PALETTE_SIZE 32
#define PALETTE_OFFSET_BYTES 128
#define SPRITE_HEADER_BYTES (PALETTE_OFFSET_BYTES + (PALETTE_SIZE * 4))

class SparkboxVideo {
public:
  SparkboxVideo();
  ~SparkboxVideo();

  int32_t loadAllSprites(std::string directory);
  int32_t addSprite(std::string filePath);
  void syncAllVideo();

  
  // Add layers to the layer bank
  int32_t addAllSprites(std::string rootDirectory);
  int32_t addSprite(std::string file);
  uint8_t addText(std::string textString);

  // Get layers in layer bank to make active
  Layer getLayerAt(uint8_t layerID);

  // Layer synchronozation functions between GPU and CPU
  void synchronizeActiveLayers(void);
  void writeActiveLayers(void);

  void smartUpdateLayerHeadersFromGpu(void);
  void updateLayerHeadersFromGpu(void);
  void updateLayerHeadersFromCpu(void);
  void updateLayerPalettesFromCpu(void);

  // Select up to 32 of the 256 total layers to be active
  // The user may have multiple instances of layers in the bank
  std::vector<Layer> activeLayers;
private:
  // layerBank populated by loadAllVideo, add
  std::vector<Layer> layerBank;

  // Keep track of the total bytes written to the GPU RAM
  uint32_t totalImportedFileSizeBytes;
  SparkboxGpu gpu;
}