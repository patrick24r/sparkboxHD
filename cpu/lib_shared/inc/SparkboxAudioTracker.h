#pragma once
#include <vector>
#include <cstdint>

// This class is a simple container for all information needed to play
// a single audio file from the external flash memory.
class SparkboxAudioTracker
{
  SparkboxAudioTracker(uint32_t bufferSizeBytes)
  {
    // Allocate internal memory for sample buffers
    internalMemoryBuffer.resize(bufferSizeBytes, 0);

    // By default, audio is not playing
    isPlaying = 0;
    numberOfRepeats = 0;
    lowerSampleRateCounter = 0;

    // The calling function must later define where the external data is
    externalMemoryBuffer = 0;
    nextExternalByteToRead = 0;

    // Initialize pointers to internal buffers.
    internalBufferFirstHalf = internalMemoryBuffer.data();
    internalBufferSecondHalf = internalBufferFirstHalf + (bufferSizeBytes / 2);

    // Pointer to the next sample to play in internal memory.
    // By comparing this to the half buffer pointers, you can determine
    // which of the two half buffers is playing
    nextInternalSampleToPlay = internalBufferFirstHalf;

    // Indicate neither buffer is ready for playing
    currentHalfBufferReady = 0;
    nextHalfBufferFilled = 0;
  }

  // Resets all tracking information to begin playing the waveform from the start
  void resetAllTracking(void)
  {
    // Set the audio file to not be playing
    isPlaying = 0;
    numberOfRepeats = 0;
    lowerSampleRateCounter = 0;

    // With a reset, there's no defined external memory anymore
    externalMemoryBuffer = 0;
    nextExternalByteToRead = 0;

    // Reset the next sample to play to be the first in the buffer
    nextInternalSampleToPlay = internalBufferFirstHalf;

    // Indicate neither buffer is ready for playing
    currentHalfBufferReady = 0;
    nextHalfBufferFilled = 0;
  }

  // Is the file currently being played
  uint8_t isPlaying;
  // Number of times to repeat an audio file after playing it
  // Any negative number is repeat forever
  int32_t numberOfRepeats;
  // For sample rates divisible by the higher sample rates,
  // keep track of the number of higher rate samples
  uint16_t lowerSampleRateCounterMax;
  uint16_t lowerSampleRateCounter;

  // Buffer of sample data in external RAM memory
  uint8_t *externalMemoryBuffer;
  // Pointer to the next byte in external RAM to read
  uint8_t *nextExternalByteToRead;

  // Buffer of sample data in RAM memory
  std::vector<uint8_t> internalMemoryBuffer;
  // Pointer to the next sample in the internal memory buffer
  uint8_t *nextInternalSampleToPlay;

  // Pointers to the beginnning of the two sections of the internalMemoryBuffer
  uint8_t *internalBufferFirstHalf;
  uint8_t *internalBufferSecondHalf;

  // State variable for handling DMA transfers, determines if
  // the next half of the buffer is filled with valid data for the
  // next samples
  uint8_t currentHalfBufferReady;
  uint8_t nextHalfBufferFilled;
};