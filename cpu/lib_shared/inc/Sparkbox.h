#pragma once
#include <vector>
#include <string>
#include "SparkboxVideo.h"
#include "SparkboxAudio.h"
#include "SparkboxController.h"

// Enumeration of all possible Sparkbox error codes
enum errorCode_t : int32_t {
  NO_ERROR = 0
};

// This class is a container for all Sparkbox peripherals
class Sparkbox 
{
public:
  Sparkbox(std::string *levelDirectory);
  ~Sparkbox();

  // Import all level data on disk for the current level
  importLevel(void);

  /* Sparkbox Handles */
  // Sparkbox GPU interface handle
  SparkboxGpu gpu;
  // Sparkbox audio interface handle
  SparkboxAudio audio;
  // Sparkbox controller interface handle
  SparkboxController controller;
  // Sparkbox fatfs interface handle
  SparkboxFatfs fatfs;
private:
  /* Sparkbox private methods */

  /* Sparkbox private properties */
  // Root directory of the current level
  std::string rootLevelDirectory;
}