#ifndef INC_SPARKBOXAUDIOMANAGER_H_
#define INC_SPARKBOXAUDIOMANAGER_H_

#include <vector>
#include <queue>
#include <string>
#include "ff.h"
#include "sparkboxErrors.hpp"

#ifndef MAX_AUDIO_STREAMS
#define MAX_AUDIO_STREAMS (4)
#endif // !MAX_AUDIO_STREAMS

#ifndef MAX_AUDIO_FILES
#define MAX_AUDIO_FILES (256UL)
#endif // !MAX_AUDIO_FILES

#ifndef MAX_SAMPLE_FREQUENCY
#define MAX_SAMPLE_FREQUENCY (48000UL)
#endif // !MAX_SAMPLE_FREQUENCY

#ifndef STREAM_BUFFER_SIZE_BYTES
#define STREAM_BUFFER_SIZE_BYTES (1024UL)
#endif // !STREAM_BUFFER_SIZE_BYTES

#ifndef ALL_SAMPLES_BUFFER_SIZE_BYTES
#define ALL_SAMPLES_BUFFER_SIZE_BYTES (0x10000000UL)
#endif // !ALL_SAMPLES_BUFFER_SIZE_BYTES

using namespace std;

typedef struct SparkboxAudioDriver {
	unsigned char* allSamplesBuffer; //< Buffer for all imported raw samples
	sparkboxError_t(*hostInitialize)(void); //< Initialize all host peripherals
	sparkboxError_t(*hostWriteSample)(unsigned int); //< Write the sample to the output
	sparkboxError_t(*hostBeginDMATx)(void*, void*, unsigned int); //< Begin DMA transfer from allSamplesBuffer to 
	void (*hostDeinitialize)(void); //< Deinitialize all host peripherals
} SparkboxAudioDriver_TypeDef;

class SparkboxAudioManager
{
public:
	SparkboxAudioManager();
	~SparkboxAudioManager();

	sparkboxError_t linkDriver(const SparkboxAudioDriver_TypeDef* audioDriver);
	sparkboxError_t initialize();

	// Import audio
	sparkboxError_t importAllAudioFiles(string directoryPath);
	sparkboxError_t importAudioFile(string filePath);

	sparkboxError_t setAudioStream(unsigned char audioStream, string audioFilePath);
	sparkboxError_t resetAudioStream(unsigned char audioStream);

	sparkboxError_t playAudioStream(unsigned char audioStream, int numberOfPlays);
	sparkboxError_t stopAudioStream(unsigned char audioStream);
	sparkboxError_t resumeAudioStream(unsigned char audioStream);
	sparkboxError_t rewindAudioStream(unsigned char audioStream);
	sparkboxError_t rewindAudioStream(unsigned char audioStream, float numberOfSeconds);
	sparkboxError_t rewindAudioStream(unsigned char audioStream, int numberOfSamples);
	sparkboxError_t skipAudioStream(unsigned char audioStream, float numberOfSeconds);
	sparkboxError_t skipAudioStream(unsigned char audioStream, int numberOfSamples);


	// Callback definitions
	void timerInterruptCallback();
	void DMATransferCompleteCallback();
	// Main audio thread function
	void audioThreadFunction();
private:
	/* Private classes */
	class ImportedAudioFile
	{
	public:
		unsigned char numberOfChannels; // 1 = Mono, 2 = Stereo
		unsigned int sampleRate; // Samples per second
		unsigned char bytesPerSample; // bytes per channel per sample
		unsigned int numberOfSamples; // Total number of samples
		unsigned int blockAlign; // bytes per sample * numChannels
		string importedFilePath; // Disk file path to source file
		unsigned char* externalDataAddress; // SDRAM address
		unsigned int dataSizeBytes; // Total external storage size in bytes
	};

	class AudioStreamTracker
	{
	public:
		// Index of the current audio file being played. -1 if not assigned
		short audioFileIndex = -1;
		// 0 = user does not want stream playing, 1 - user wants stream playing
		unsigned char playRequested = 0;
		// 0 = audio not playing, 1 = audio playing
		unsigned char isPlaying = 0;
		// Number of times to repeat an audio file after playing it
		// Any negative number is repeat forever
		int playsRemaining = 0;
		// Track the number of samples left before the end of the audio file
		unsigned int samplesRemaining;
		// Track files with sample rates some fraction of the master rate
		unsigned short sampleRateDivisor = 1;
		unsigned short sampleRateCounter = 0;

		unsigned char* nextSampleToRead;
		unsigned char* activeSample;
		unsigned char* nextInternalSampleToFill;
		unsigned char internalBuffer[STREAM_BUFFER_SIZE_BYTES];
	};

	class AudioDmaRequest
	{
	public:
		unsigned char* transferFromAddress; // External buffer memory address
		unsigned char* transferToAddress; // Internal memory buffer address
		unsigned int transferSizeBytes; // Transfer size in bytes
	};

	unsigned char isInitialized = 0;
	unsigned char streamsLocked = 0;
	unsigned char dmaTransferActive = 0;
	unsigned int totalAudioBytesImported = 0;

	const SparkboxAudioDriver_TypeDef* driver;
	vector<ImportedAudioFile> importedAudioFiles;
	vector<AudioStreamTracker> audioStreamTrackers;
	queue<AudioDmaRequest> dmaRequestQueue;

	sparkboxError_t setAudioStream(unsigned char audioStream, unsigned int audioFileID);

	sparkboxError_t importWAVFile(string filePath);

	int mixAudioSample();
};

#endif /* INC_SPARKBOXAUDIOMANAGER_H_ */
