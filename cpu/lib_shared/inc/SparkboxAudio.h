#pragma once
#include "allPeripherals.h"
#include "AudioFile.h"
#include "SparkboxAudioTracker.h"
#include "SparkboxDmaRequest.h"
#include <vector>
#include <queue>

#define MAX_AUDIO_CHANNELS 4
#define MAX_AUDIO_BANK_SIZE 256
#define SINGLE_AUDIO_BUFFER_BYTES 1024
#define TOTAL_AUDIO_BUFFER_SIZE_BYTES (MAX_AUDIO_CHANNELS * SINGLE_AUDIO_BUFFER_BYTES)
#define AUDIO_START_ADDR 0x70000000UL
#define AUDIO_MAX_SIZE_BYTES 0x0FFFFFFFUL

class SparkboxAudio
{
  SparkboxAudio(void);
  ~SparkboxAudio();

  int32_t addAllAudio(std::string directory);
  int32_t addAudioFile(std::string filePath);

  // Audio controlling functions
  int32_t playAudio(uint8_t audioChannel);
  int32_t playAudio(uint8_t audioChannel, int32_t numberOfPlays);
  int32_t pauseAudio(uint8_t audioChannel);
  int32_t resumeAudio(uint8_t audioChannel);

  // Functions to rewind or skip audio
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
  // Handle to DAC peripheral for initialization
  DAC_HandleTypeDef hdac;
  TIM_HandleTypeDef htim11;
  TIM_HandleTypeDef htim14;

  // Keep track of the total imported audio sample size in bytes
  uint32_t totalImportedAudioBytes;
  // Bank of all imported audio file headers - Up to 256
  std::vector<AudioFile> audioBank;
  // Contains up to 4 active audio file headers
  std::vector<AudioFile> activeAudio;
  // Container for all internal data to manage playing all audio
  std::vector<SparkboxAudioTracker> audioTrackers;
  // Current active audio samples
  std::vector<uint32_t> activeSamples;
  // Queue of requests for dma to transfer data on a given channel
  std::queue<SparkboxDmaRequest> channelDataRequests;

  // Master volume of the audio
  float masterVolume;
  
  // Add different supported audio file formats,
  // converting to unsigned 8 or 16 bit mono or stereo audio
  int32_t addWavFile(std::string filePath);
  int32_t addMp3File(std::string filePath);

  // Audio interrupt callbacks and related functions
  void audioInterruptCallback(uint32_t itSampleRate);
  // Get the next sample from internal memory
  void getNextSample(uint8_t channel);
  // Write the active sample data to the DACs
  void writeNewSampleDacs(void);
  // Audio DMA complete interrupt callback
  void dmaCompleteCallback(void);
  // Update all audio data streams
  void updateAudioStreams(void);
};