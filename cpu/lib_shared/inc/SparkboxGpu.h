// This class contains low level functions to interface
// with the Sparkbox GPU
#pragma once
#include "allPeripherals.h"
#include "Layer.h"

#define MAX_LAYER_COUNT 256
#define MAX_ACTIVE_LAYERS 32
#define SPRITE_BUFFER_SIZE 2048

class SparkboxGpu
{
public:
  // Constructor and Destructor
  SparkboxGpu();
  ~SparkboxGpu();

  // Enumerations for building commands
  enum commandType : uint16_t {
    TYPE_SPECIAL = 0b00 << 14,
    TYPE_READ =    0b01 << 14,
    TYPE_WRITE =   0b10 << 14,
    TYPE_RESET =   0b11 << 14
  };

  enum commandTarget : uint16_t {
    TARGET_ALL =          0b000 << 11,
    TARGET_LAYERHEADERS = 0b001 << 11,
    TARGET_RAM =          0b010 << 11,
    TARGET_PALETTE =      0b011 << 11,
    TARGET_FLASH =        0b100 << 11
  };

  enum layerTarget : uint16_t {
    ALL_LAYERS = 0x20
  };

  enum specialCommands : uint16_t {
    NO_OP = 0,
    FRAME_UPDATE = 1
  };

  // Commands to read/write/reset layer headers
  void resetLayerHeaders(void);
  void resetLayerHeaders(uint8_t layer);
  uint16_t readLayerHeaders(uint8_t layer, uint8_t registerIndex);
  void writeLayerHeaders(uint8_t layer, uint8_t registerIndex, uint16_t data);

  // Commands to read/write/reset RAM
  void resetRam(void);
  void readRam(uint8_t layerID, uint16_t *dataToRead, uint16_t size);
  void writeRam(uint8_t layerID, uint16_t *dataToWrite, uint16_t size);

  // Commands to read/write/reset Flash
  void resetFlash(void);
  void readFlash(uint16_t fontIndex, uint16_t *dataToRead, uint16_t size);
  void writeFlash(uint16_t fontIndex, uint16_t *dataToWrite, uint16_t size);

  // Commands to read/write/reset palette
  void resetPalette(void);
  uint32_t readPalette(uint8_t layer, uint8_t colorSlot);
  void writePalette(uint8_t layer, uint8_t colorSlot, uint32_t color);

  // Other commands
  void resetAll(void);
  void startFrameUpdate(void);

  // Additional functions
  uint8_t isBusy(void);
  void waitUntilFree(void);

  // Add or remove layers
  int32_t addAllSprites(std::string rootDirectory);
  int32_t addSprite(std::string file);
  uint8_t addText(std::string textString);

  // Layer synchronozation functions between GPU and CPU
  void synchronizeActiveLayers(void);
  void writeActiveLayers(void);

  void smartUpdateLayerHeadersFromGpu(void);
  void updateLayerHeadersFromGpu(void);
  void updateLayerHeadersFromCpu(void);

  void updateLayerPalettesFromCpu(void);

  // Select up to 32 of the 256 total layers to be active
  // The user may select multiple instances of imported layers as active layers
  std::vector<Layer> activeLayers;

  // Send raw commands to the GPU
  // Only use if you really know what you're doing
  uint16_t sendRawCommand(uint16_t command, uint16_t data);
private:
  // Hold up to 256 total layers
  std::vector<Layer> importedLayers;
  
  // Keep track of the total memory required for all layers
  uint32_t totalImportedFileSizeBytes;

  // Send GPU command functions
  uint16_t sendGpuCommand(uint16_t command);
  uint16_t sendGpuCommand(uint16_t command, uint16_t data);

}