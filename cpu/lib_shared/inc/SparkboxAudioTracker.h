#pragma once
#include <vector>


// This class is a simple container for all information needed to play
// a single audio file from the external flash memory. 
class SparkboxAudioTracker
{
  SparkboxAudioTracker(uint32_t bufferSizeBytes);

  // Resets all tracking information to begin playing the waveform from the start
  void resetAllTracking(void);

  // Is the file currently being played
  uint8_t isPlaying;
  // Number of times to repeat an audio file after playing it
  // Any negative number is repeat forever
  int32_t numberOfRepeats;
  // For sample rates divisible by the higher sample rates,
  // keep track of the number of higher rate samples
  uint32_t lowerSampleRateCounter;

  // Buffer of sample data in flash memory
  uint8_t *externalMemoryBuffer;
  // Pointer to the next byte in flash memory to read
  uint8_t *nextExternalByteToRead;

  // Buffer of sample data in RAM memory
  std::vector<uint8_t> internalMemoryBuffer;
  // Pointer to the next sample in the internal memory buffer
  uint8_t *nextInternalSampleToPlay;

  // State variable for handling DMA transfers, determines if
  // the next half of the buffer is filled with valid data for the 
  // next samples
  uint8_t currentHalfBufferReady;
  uint8_t nextHalfBufferFilled;
  // Pointers to the beginnning of the two sections of the internalMemoryBuffer
  uint8_t *internalBufferFirstHalf;
  uint8_t *internalBufferSecondHalf;
};