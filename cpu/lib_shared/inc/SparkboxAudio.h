#pragma once
#include "allPeripherals.h"
#include "AudioFile.h"
#include <vector>

#define MAX_AUDIO_CHANNELS 4
#define MAX_AUDIO_BANK_SIZE 256
#define AUDIO_START_ADDR 0x70000000UL
#define AUDIO_MAX_SIZE_BYTES 0x0FFFFFFFUL

class SparkboxAudio
{
  SparkboxAudio(void);
  ~SparkboxAudio(void);

  int32_t addAllAudio(std::string directory);
  int32_t addAudioFile(std::string filePath);

  // Audio controlling functions
  int32_t playAudio(uint8_t audioChannel);
  int32_t playAudio(uint8_t audioChannel, int32_t numberOfPlays);
  int32_t pauseAudio(uint8_t audioChannel);
  int32_t resumeAudio(uint8_t audioChannel);
  int32_t rewindAudio(uint8_t audioChannel);
  int32_t rewindAudio(uint8_t audioChannel, float numberOfSeconds);
  int32_t rewindAudio(uint8_t audioChannel, int32_t numberOfSamples);
  int32_t skipAudio(uint8_t audioChannel, float numberOfSeconds);
  int32_t skipAudio(uint8_t audioChannel, int32_t numberOfSamples);
  void setChannelVolume(uint8_t audioChannel, float newVolume);

  float getMasterVolume(void);
  void setMasterVolume(float newVolume);
  int32_t setActiveAudio(uint8_t audioChannel, uint8_t audioID);
  AudioFile getAudioBank(uint8_t audioID);

private:
  // Contains up to 4 active audio file headers
  std::vector<AudioFile> activeAudio;

  // Bank of all imported audio file headers - Up to 256
  std::vector<AudioFile> audioBank;

  // Master volume of the audio
  float masterVolume;

  // Keep track of the tomtal imported audio
  uint32_t totalImportedAudioBytes;

  // Buffer of raw audio samples
  std::vector<uint32_t> channelSamples;

  // Buffer of audio sample rate counters
  std::vector<uint8_t> rateCounter;

  // Handle to DAC peripheral for initialization
  DAC_HandleTypeDef hdac;
  TIM_HandleTypeDef htim11;
  TIM_HandleTypeDef htim14;

  // Add different supported audio file formats,
  // converting to unsigned 8 or 16 bit mono or stereo audio
  int32_t addWavFile(std::string filePath);
  int32_t addMp3File(std::string filePath);

  // Audio interrupt callback
  void audioInterruptCallback(uint32_t itSampleRate);
  void getNewSample(uint8_t channel);
}