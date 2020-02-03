#pragma once
#include <string>

#define MAX_SAMPLE_RATE 48000UL
#define MAX_VOLUME 1.0
#define MIN_VOLUME 0.0
#define AUDIO_PLAYING 1
#define AUDIO_NOT_PLAYING 0
#define AUDIO_STOPPED AUDIO_NOT_PLAYING

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

    // By default, the volume is maximized
    volume = MAX_VOLUME;
    // By default, the audio file is not playing
    isPlaying = AUDIO_NOT_PLAYING;
  }
  ~AudioFile();

  // Mono audio - 1 channel, Stereo audio - 2 channels
  audiochannel_t numberOfChannels;
  // Bits for a single sample's worth of audio data
  samplebit_t bitsPerSample;
  // Sample rate in samples / second
  uint32_t sampleRate;
  // Index of the next sample to be played
  uint32_t nextSample;
  // Path to the audio file on disk
  std::string filePath;
  
  // Is the file currently being played
  uint8_t isPlaying;
  // Number of times to repeat an audio file after playing it
  // Any negative number is repeat forever
  int32_t numberOfRepeats;

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

  // Get the current volume level
  float getVolume(void) {
    return volume;
  }

  // Bounds check the volume to prevent clipping
  void setVolume(float newVolume) {
    // Coerce volume to [0, 1.0] range
    if (newVolume < 0.0) newVolume = 0.0;
    if (newVolume > 1.0) newVolume = 1.0;
    volume = newVolume;
  }

private:
  // Volume - private to coerce value between -1.0 and 1.0
  float volume;
  // Address to beginning of audio sample data
  uint8_t *dataAddress;
  // Total number of samples
  uint32_t numberOfSamples;
}