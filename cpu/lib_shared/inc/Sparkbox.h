#pragma once
#include "Layer.h"
#include "SparkboxGpu.h"
#include "SparkboxAudio.h"
#include "SparkboxController.h"
#include <vector>
#include <string>


#define MAX_LAYER_COUNT 256
#define MAX_ACTIVE_LAYERS 32
#define SPRITE_BUFFER_SIZE 2048

// This class is a container for all Sparkbox peripherals
class Sparkbox 
{
public:
  Sparkbox(std::string *levelDirectory);
  ~Sparkbox();

  // Enumeration of all possible Sparkbox error codes
  enum errorCode_t : int32_t {
    NO_ERROR = 0
  };

  // Import any individual sprite from disk
  errorCode_t importSprite(std::string filePath);

  // Synchronize layer headers between 
  errorCode_t synchronizeLayers(void);

private:
  /* Sparkbox private methods */
  // Import sprites in rootDirectory into cpu and gpu memory
  errorCode_t importLevel(void);

  /* Sparkbox Handles */
  // Sparkbox GPU interface handle
  SparkboxGpu *gpu;
  // Sparkbox audio interface handle
  SparkboxAudio *audio;
  // Sparkbox controller interface handle
  SparkboxController *controller;
  // Hold up to 256 total layers
  std::vector<Layer> layers;
  // Select up to 32 of the 256 total layers to be active
  std::vector<uint8_t> activeLayers;

  /* Sparkbox private properties */
  // Root directory of the current level
  std::string rootDirectory;
  uint32_t totalImportedFileSizeBytes;
}