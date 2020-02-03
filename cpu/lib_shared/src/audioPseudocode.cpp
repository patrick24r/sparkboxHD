
/* Audio is always being transferred over SPI to FPGA for mixing and decoding
 * in a continuous circular DMA buffer. This is possible because each audio
 * sampleis limited to at most 32 bits on this 32 bit processor
  * (16 bit channel * 2 channels)
 */

// Channel X timer callback
void chxtimerCallback(channel x)
{
  uint32_t newChannelSample;

  // If not currently playing, do nothing
  if (!activeAudio.at(x).isPlaying) return;

  // Send new ch1 sample to SPI buffer
  // Find byte offset of next sample in storage
  byteOffset = activeAudio.at(x).sampleOffset * bytesPerSample;

  // Based on how sample data is stored (8 vs 16 bit, mono vs stereo),
  // determine how to move data from sample storage to dma buffer
  newChannelSample = *(uint32_t*)(activeAudio.at(x).sampleDataPtr + byteOffset);

  // If the audio is mono, set both channels to the 16 lsb of the data read
  if (activeAudio.at(x).numberOfChannels == 1) {
    // Clear upper 16 bits
    newChannelSample &= ~(0xFFFF0000);
    // Copy lower bits to upper bits
    newChannelSample |= (newChannelSample << 16);
  }

  // Only 8 bit audio, need to shift 8 lsb to 8 msb
  if (activeAudio.at(x).bitsPerSample == 8) {
    // Clear 8 MSB of sample (L and R)
    newChannelSample &= ~(0xFF00FF00);
    // Shift lsb to msb
    newChannelSample <<= 16;
  }

  // Write to new dma location
  *chxdmalocation = newChannelSample;

  // Check if we're done playing this audio file
  if (++activeAudio.at(x).sampleOffset > activeAudio.at(x).totalSamples) {
    activeAudio.at(x).sampleOffset = 0;

    // Done playing, if no more repeats, stop playing the audio
    activeAudio.at(x).finishedAudioFile();
  }
}

// DMA transfer to SPI is done
void audioDmaDoneCallback(void)
{
  // As long as no error exists, KEEP 'ER ROLLING, BUD
}

/* SCRAPPED AUDIO ALGORITHM
DUE TO OVERWHELMING USAGE OF FPU AND CPU */
// 1 timer to handle sample updates
// 1 timer to handle
void timerInterruptCallback(void)
{
  bool triggerSpiDma = false;
  uint32_t byteOffset = 0;

  // Find the time between sample frequency updates
  float ch1sampleTime = 1/ activeAudio.at(1).sampleRate;
  float ch2sampleTime = 1/ activeAudio.at(2).sampleRate;
  float ch3sampleTime = 1/ activeAudio.at(3).sampleRate;
  float ch4sampleTime = 1/ activeAudio.at(4).sampleRate;

  // Find the time between timer interrupts
  float timerTime = 1/ timerFreq;


  // timerTime < ch[1-4]sampleTime

  //ch1
  if (ch1active) {
    // Add new time to offset
    ch1timeoffset += timerTime;
    // On offset overflow, we have a new sample
    if (ch1timeoffset > ch1sampleTime) {
      // Carry over time offset
      ch1timeoffset -= ch1sampleTime;

      // Send new ch1 sample to SPI buffer
      // Find byte offset of next sample in storage
      byteOffset = activeAudio.at(1).sampleOffset * bytesPerSample;


      // With new sample, trigger
      triggerSpiDma = true;
    } else {
      ch1timeoffset += timerTime;
    }
  }
}
