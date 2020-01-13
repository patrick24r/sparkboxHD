// This class contains low level functions to interface
// with the Sparkbox GPU
#pragma once
#include "allPeripherals.h"
#include "Layer.h"

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

  // Determine if the GPU is busy or not
  uint8_t isBusy(void);
  void waitUntilFree(void);

  std::vector<Layer> activeLayers;

  // Send raw commands to the GPU
  // Only use if you really know what you're doing
  uint16_t sendRawCommand(uint16_t command, uint16_t data);
private:
  // Send GPU command functions
  uint16_t sendGpuCommand(uint16_t command);
  uint16_t sendGpuCommand(uint16_t command, uint16_t data);

}