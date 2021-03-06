#pragma once
#include <string>

#define MAX_SAMPLE_RATE 48000UL

// Container for audio header data
class AudioFile {
public:
  typedef enum audiochannel_t : uint8_t {
    CHANNEL_MONO = 1,
    CHANNEL_STEREO = 2
  };

  typedef enum samplebit_t : uint8_t {
    BITS_PER_SAMPLE_8 = 8,
    BITS_PER_SAMPLE_16 = 16
  };

  AudioFile(uint8_t *sampleDataLocation, uint32_t totalSampleNumber)
  {
    // Save the location of the data and how large it is
    dataAddress = sampleDataLocation;
    numberOfSamples = totalSampleNumber;
  }
  ~AudioFile();

  // Mono audio - 1 channel, Stereo audio - 2 channels
  audiochannel_t numberOfChannels;
  // Bits for a single sample of a single channel
  samplebit_t bitsPerSample;
  // Sample rate in samples / second
  uint32_t sampleRate;
  // Path to the audio file on disk
  std::string filePath;

  // Return the start address to the sample data
  uint8_t * getDataAddress(void)
  {
    return dataAddress;
  }

  // Return the sample data size in bytes
  uint32_t getNumberOfSamples(void)
  {
    return numberOfSamples;
  }

  // Get the number of bytes for a single sample (both channels)
  uint32_t getBytesPerSample(void)
  {
    return (bitsPerSample >> 3) * numberOfChannels;
  }

private:
  // Address to beginning of audio sample data
  uint8_t *dataAddress;
  // Total number of samples
  uint32_t numberOfSamples;
};