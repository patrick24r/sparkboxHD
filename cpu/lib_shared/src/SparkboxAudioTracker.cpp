#pragma once
#include "SparkboxAudioTracker.h"

SparkboxAudioTracker::SparkboxAudioTracker(uint32_t bufferSizeBytes)
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

// This function is used to reset all data from 
void SparkboxAudioTracker::resetAllTrackers(void)
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