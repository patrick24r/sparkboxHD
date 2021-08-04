/*
 * SparkboxAudioManager.h
 *
 *  Created on: Jul 30, 2021
 *      Author: patri
 */

#ifndef INC_SPARKBOXAUDIOMANAGER_H_
#define INC_SPARKBOXAUDIOMANAGER_H_

#include "stm32h7xx_hal.h"
#include "dac.h"
#include "dma.h"
#include "fmc.h"
#include "tim.h"
#include <vector>
#include <queue>
#include <string>


using namespace std;

extern "C" void cb_dmaComplete(void);
extern "C" void cb_audioTimerIT(void);

class SparkboxAudioManager
{
public:
	const uint32_t MaxAudioStreams = 4;
	const uint32_t MaxAudioFiles = 256;
	const uint32_t TimerFrequency = 48000;

	SparkboxAudioManager(void);
	~SparkboxAudioManager();

	int32_t playStream(uint8_t audioStream);
	int32_t playStream(uint8_t audioStream, int32_t numberOfPlays);



	void callback_AudioTimerIT(void);
	void callback_DMATransferComplete(void);
private:
	// Constants used for managing data storage both for 
	// internal RAM and external RAM
	const uint8_t* ExternalBufferAddress = (uint8_t*)0xC0000000;
	const uint32_t ExternalBufferBytes = 0x10000000;
	const uint32_t InternalSingleBufferBytes = 1024;
	
	/* Private classes */
	
	class AudioDmaRequest 
	{
	public:
		uint8_t* transferFromAddress;
		uint8_t* transferToAddress;
		uint32_t transferSizeBytes;
		int8_t transferFillsBuffer;
	};

	class ImportedAudioFile
	{
	public:
		uint8_t numberOfChannels; // 1 = Mono, 2 = Stereo
		uint32_t sampleRate; // Samples per second
		uint8_t bytesPerSample; // bytes per channel per sample
		uint32_t numberOfSamples; // Total number of samples
		uint32_t blockAlign; // bytes per sample * numChannels
		
		string importedFilePath; // Disk file path to source file
		uint8_t* externalDataAddress; // SDRAM address
		uint32_t dataSizeBytes; // Total external storage size in bytes
	};

	class AudioStreamTracker
	{
	public:
		AudioStreamTracker(uint32_t bufferSizeBytes)
		{
			internalBuffer = (uint8_t*)malloc(sizeof(uint8_t) * bufferSizeBytes);
		}
		~AudioStreamTracker()
		{
			free(internalBuffer);
		}
		
		// Index of the current audio file being played. -1 if not assigned
		int16_t audioFileIndex;
		// 0 = audio not playing, 1 = audio playing
		uint8_t isPlaying = 0;
		// 0 = current buffer is not ready, 
		uint8_t bufferReady = 0;
		// Number of times to repeat an audio file after playing it
		// Any negative number is repeat forever
		int32_t repeatsRemaining;
		// Track the number of samples left before the end of the audio file
		uint32_t samplesRemaining;
		// Track files with sample rates some fraction of the master rate
		uint16_t sampleRateDivisor = 1;
		uint16_t sampleRateCounter = 0;

		uint8_t* nextExternalSampleToRead;
		uint8_t* activeSample;
		uint8_t* internalBuffer;
	};


	// Bits [31:16] Left channel sample (uint16)
	// Bits [15:0] Right channel sample (uint16)
	uint32_t activeSamples;

	uint32_t totalAudioBytesImported = 0;
	vector<AudioStreamTracker> audioPlayerTrackers;
	vector<ImportedAudioFile> importedAudioFiles;
	queue<AudioDmaRequest> dmaRequestQueue;
};

extern SparkboxAudioManager spkAudioManager;


#endif /* INC_SPARKBOXAUDIOMANAGER_H_ */
