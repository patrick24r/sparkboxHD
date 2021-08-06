#ifndef INC_SPARKBOXAUDIOMANAGER_H_
#define INC_SPARKBOXAUDIOMANAGER_H_

extern "C" {
#include "stm32h7xx_hal.h"
#include "cmsis_os.h"
#include "dac.h"
#include "mdma.h"
#include "fmc.h"
#include "tim.h"
#include "fatfs.h"
}

#include <vector>
#include <queue>
#include <string>

using namespace std;

class SparkboxAudioManager
{
public:
	const uint32_t MaxAudioStreams = 4;
	const uint32_t MaxAudioFiles = 256;
	const uint32_t MaxSampleFrequency = 48000;

	// Singleton class instance getter
	static SparkboxAudioManager* getInstance();
	~SparkboxAudioManager();

	// Import audio
	int32_t importAllAudioFiles(string directoryPath);
	int32_t importAudioFile(string filePath);

	int32_t setAudioStream(uint8_t audioStream, string audioFilePath);
	int32_t setAudioStream(uint8_t audioStream, uint8_t audioFileID);
	int32_t resetAudioStream(uint8_t audioStream);

	int32_t playAudioStream(uint8_t audioStream);
	int32_t playAudioStream(uint8_t audioStream, int32_t numberOfPlays);
	int32_t stopAudioStream(uint8_t audioStream);
	int32_t resumeAudioStream(uint8_t audioStream);
	int32_t rewindAudioStream(uint8_t audioStream);
	int32_t rewindAudioStream(uint8_t audioStream, float numberOfSeconds);
	int32_t rewindAudioStream(uint8_t audioStream, int32_t numberOfSamples);
	int32_t skipAudioStream(uint8_t audioStream, float numberOfSeconds);
	int32_t skipAudioStream(uint8_t audioStream, int32_t numberOfSamples);


	// Callbacks for interrupts
	void callback_AudioTimerIT(void);
	void callback_DMATransferComplete(void);
	// Main audio thread function
	void threadfcn_AudioManager(void);
private:
	/* Private classes */
	class AudioDmaRequest
	{
	public:
		// External buffer memory address
		uint8_t* transferFromAddress;
		// Internal memory buffer address
		uint8_t* transferToAddress;
		// Transfer size in bytes
		uint32_t transferSizeBytes;
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
		int16_t audioFileIndex = -1;
		// 0 = user does not want stream playing, 1 - user wants stream playing
		uint8_t playRequested = 0;
		// 0 = audio not playing, 1 = audio playing
		uint8_t isPlaying = 0;
		// Number of times to repeat an audio file after playing it
		// Any negative number is repeat forever
		int32_t playsRemaining = 0;
		// Track the number of samples left before the end of the audio file
		uint32_t samplesRemaining;
		// Track files with sample rates some fraction of the master rate
		uint16_t sampleRateDivisor = 1;
		uint16_t sampleRateCounter = 0;

		uint8_t* nextExternalSampleToRead;
		uint8_t* activeSample;
		uint8_t* nextInternalSampleToFill;
		uint8_t* internalBuffer;
	};

	// Make this a singleton class
	SparkboxAudioManager();
	// private copy-constructor (left undefined):
	SparkboxAudioManager(const SparkboxAudioManager&);
	// private copy-assignment operator (left undefined):
	SparkboxAudioManager& operator=(const SparkboxAudioManager&);

	// Constants used for managing data storage both for
	// internal RAM and external RAM
	const uint8_t* ExternalBufferAddress = (uint8_t*)0xC0000000;
	const uint32_t ExternalBufferBytes = 0x10000000;
	const uint32_t InternalSingleBufferBytes = 1024;

	osMutexId_t spkAudioTrackMutexHandle;
	const osMutexAttr_t spkAudioTrackMutex_attributes = {
		.name = "spkAudioTrackMutex"
	};
	uint32_t totalAudioBytesImported = 0;
	uint8_t dmaTransferActive = 0;
	vector<AudioStreamTracker> audioStreamTrackers;
	vector<ImportedAudioFile> importedAudioFiles;
	queue<AudioDmaRequest> dmaRequestQueue;

	void mixAudioSample(uint16_t& leftSample, uint16_t& rightSample);
};

#endif /* INC_SPARKBOXAUDIOMANAGER_H_ */
