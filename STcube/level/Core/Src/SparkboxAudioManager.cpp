#include "SparkboxAudioManager.h"

SparkboxAudioManager::SparkboxAudioManager(void)
{
	// Initialize the internal audio buffers and trackers
	for (uint i = 0; i < MaxAudioStreams; i++) {
		audioStreamTrackers.push_back(AudioStreamTracker(InternalSingleBufferBytes));
	}

	// Initialize relevant peripherals
	MX_FMC_Init();
	MX_DAC1_Init();
	MX_MDMA_Init();
	MX_TIM6_Init();
	hsdram1.hmdma = &hmdma_mdma_channel40_sw_0; // Link the mdma to the sdram

	// Create the mutex
	spkAudioTrackMutexHandle = osMutexNew(&spkAudioTrackMutex_attributes);

	// Start the audio output by repeatedly sending whatever is in the activeSamples variable
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
}

SparkboxAudioManager::~SparkboxAudioManager()
{
	HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1);
	HAL_DAC_Stop(&hdac1, DAC_CHANNEL_2);
}


int32_t SparkboxAudioManager::importAllAudioFiles(string directoryPath)
{
	return 0;
}

int32_t SparkboxAudioManager::importAudioFile(string filePath)
{
	return 0;
}

int32_t SparkboxAudioManager::setAudioStream(uint8_t audioStream, string audioFilePath)
{
	// Try to match the audio file path
	for (auto it = importedAudioFiles.begin(); it != importedAudioFiles.end(); ++it)
	{
		if (audioFilePath == it->importedFilePath) {
			return setAudioStream(audioStream, (uint8_t)(it - importedAudioFiles.begin()));
		}
	}
	return -1;
}

int32_t SparkboxAudioManager::setAudioStream(uint8_t audioStream, uint8_t audioFileID)
{
	// acquire mutex for audioStreamTrackers
	if (osMutexAcquire(spkAudioTrackMutexHandle, osWaitForever) != osOK) return -1;

	// release mutex for audioStreamTrackers
	osMutexRelease(spkAudioTrackMutexHandle);
	return 0;
}

int32_t SparkboxAudioManager::resetAudioStream(uint8_t audioStream)
{
	// acquire mutex for audioStreamTrackers
	if (osMutexAcquire(spkAudioTrackMutexHandle, osWaitForever) != osOK) return -1;

	// Safely stop any audio currently playing
	audioStreamTrackers.at(audioStream).playRequested = 0;
	audioStreamTrackers.at(audioStream).isPlaying = 0;
	audioStreamTrackers.at(audioStream).audioFileIndex = -1;
	// Reset all other tracker values
	audioStreamTrackers.at(audioStream).playsRemaining = 0;
	audioStreamTrackers.at(audioStream).samplesRemaining = 0;
	audioStreamTrackers.at(audioStream).sampleRateDivisor = 1;
	audioStreamTrackers.at(audioStream).sampleRateCounter = 0;
	audioStreamTrackers.at(audioStream).nextExternalSampleToRead = NULL;
	audioStreamTrackers.at(audioStream).activeSample = NULL;
	audioStreamTrackers.at(audioStream).nextInternalSampleToFill = NULL;
	audioStreamTrackers.at(audioStream).internalBuffer = NULL;

	// release mutex for audioStreamTrackers
	osMutexRelease(spkAudioTrackMutexHandle);
	return 0;
}


int32_t SparkboxAudioManager::playAudioStream(uint8_t audioStream)
{
	return playAudioStream(audioStream, 1);
}

int32_t SparkboxAudioManager::playAudioStream(uint8_t audioStream, int32_t numberOfPlays)
{
	resetAudioStream(audioStream);

	// acquire mutex for audioStreamTrackers
	if (osMutexAcquire(spkAudioTrackMutexHandle, osWaitForever) != osOK) return -1;
	audioStreamTrackers.at(audioStream).playsRemaining = numberOfPlays;
	// release mutex for audioStreamTrackers
	osMutexRelease(spkAudioTrackMutexHandle);
	resumeAudioStream(audioStream);
	return 0;
}

int32_t SparkboxAudioManager::stopAudioStream(uint8_t audioStream)
{
	// acquire mutex for audioStreamTrackers
	if (osMutexAcquire(spkAudioTrackMutexHandle, osWaitForever) != osOK) return -1;

	// release mutex for audioStreamTrackers
	osMutexRelease(spkAudioTrackMutexHandle);
	return 0;
}

int32_t SparkboxAudioManager::resumeAudioStream(uint8_t audioStream)
{
	// acquire mutex for audioStreamTrackers
	if (osMutexAcquire(spkAudioTrackMutexHandle, osWaitForever) != osOK) return -1;

	// release mutex for audioStreamTrackers
	osMutexRelease(spkAudioTrackMutexHandle);
	return 0;
}

int32_t SparkboxAudioManager::rewindAudioStream(uint8_t audioStream)
{
	// acquire mutex for audioStreamTrackers
	if (osMutexAcquire(spkAudioTrackMutexHandle, osWaitForever) != osOK) return -1;


	AudioStreamTracker str = audioStreamTrackers.at(audioStream);
	str.nextExternalSampleToRead =
		importedAudioFiles.at(str.audioFileIndex).externalDataAddress;
	// release mutex for audioStreamTrackers
	osMutexRelease(spkAudioTrackMutexHandle);

	return 0;
}

int32_t SparkboxAudioManager::rewindAudioStream(uint8_t audioStream, float numberOfSeconds)
{
	return 0;
}

int32_t SparkboxAudioManager::rewindAudioStream(uint8_t audioStream, int32_t numberOfSamples)
{
	// acquire mutex for audioStreamTrackers
	if (osMutexAcquire(spkAudioTrackMutexHandle, osWaitForever) != osOK) return -1;

	// release mutex for audioStreamTrackers
	osMutexRelease(spkAudioTrackMutexHandle);
	return 0;
}

int32_t SparkboxAudioManager::skipAudioStream(uint8_t audioStream, float numberOfSeconds)
{
	return 0;
}

int32_t SparkboxAudioManager::skipAudioStream(uint8_t audioStream, int32_t numberOfSamples)
{
	// acquire mutex for audioStreamTrackers
	if (osMutexAcquire(spkAudioTrackMutexHandle, osWaitForever) != osOK) return -1;

	// release mutex for audioStreamTrackers
	osMutexRelease(spkAudioTrackMutexHandle);
	return 0;
}


// DMA transfer from external buffer to internal buffer is complete
void SparkboxAudioManager::callback_DMATransferComplete(void)
{
	AudioDmaRequest nextDmaTx;
	// Pop the request that just finished
	dmaRequestQueue.pop();
	// Begin the next dma transfer if one exists
	if (!dmaRequestQueue.empty()) {
		nextDmaTx = dmaRequestQueue.front();
		HAL_SDRAM_Read_DMA(&hsdram1, (uint32_t*)nextDmaTx.transferFromAddress,
			(uint32_t*)nextDmaTx.transferToAddress, nextDmaTx.transferSizeBytes);
	} else {
		dmaTransferActive = 0;
	}
}

// Timer interrupt callback
void SparkboxAudioManager::callback_AudioTimerIT(void)
{
	uint16_t newSampleLeft = 0, newSampleRight = 0;
	for (AudioStreamTracker& it : audioStreamTrackers) {
		if (it.audioFileIndex > (int16_t)importedAudioFiles.size() ||
			it.audioFileIndex < 0 || !it.playRequested) continue;
		ImportedAudioFile fil = importedAudioFiles.at(it.audioFileIndex);
		// Check if the audio streams are moving to the next sample
		if (it.isPlaying && ++it.sampleRateCounter == it.sampleRateDivisor) {
			it.sampleRateCounter = 0;
			// Increment the next sample pointer to the next full sample
			it.activeSample += fil.blockAlign;
			it.samplesRemaining--;

			// Check if we played the last of a audio stream and apply the audio repetition logic
			if (it.samplesRemaining == 0) {
				// Reset the number of samples yet to play
				it.samplesRemaining = importedAudioFiles.at(it.audioFileIndex).numberOfSamples;
				// Only decrement playsRemaining if it's positive. A negative
				// value indicates repeating forever and we hate integer overflow here
				if (it.playsRemaining > 0) it.playsRemaining--;
				// Turn off the audio if the total number of repeats is done
				if (it.playsRemaining == 0) {
					it.isPlaying = 0;
					it.playRequested = 0;
				}
			}

			// Return to the start of the internal buffer if the pointer ran past the end
			if (it.activeSample >= it.internalBuffer + InternalSingleBufferBytes) {
				it.activeSample = it.internalBuffer;
			}

			// If there is a sample overrun, stop playing
			if (it.activeSample == it.nextInternalSampleToFill) {
				it.isPlaying = 0;
			} else if (it.playRequested && !it.isPlaying) {
				// If there is a 2/3 or less buffer ready ahead of the active sample, begin playing
				if (it.activeSample < it.nextInternalSampleToFill) {
					it.isPlaying = (uint32_t)(it.nextInternalSampleToFill - it.activeSample) >= InternalSingleBufferBytes / 2;
				} else {
					it.isPlaying = (InternalSingleBufferBytes + (uint32_t)(it.nextInternalSampleToFill - it.activeSample))
						>= InternalSingleBufferBytes / 2;
				}
			}
		}
	}
	// Write newest sample value to DACs
	mixAudioSample(newSampleLeft, newSampleRight);
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_L, newSampleLeft);
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_L, newSampleRight);
}

// Main thread function
// For each audio stream
// - Check for any required dma requests
// - Check if the dma requests need to be "kicked off"
// - Check if any streams the user wants to play can be played
void SparkboxAudioManager::threadfcn_AudioManager()
{
	AudioDmaRequest req;
	uint32_t bytesOfBufferLeft, bytesOfFileLeft, bytesUntilActiveSample;
	// Repeat forever
	while (1) {
		osDelay(1); // Wait a tick (1 ms) between updates

		// acquire mutex for audioStreamTrackers
		if (osMutexAcquire(spkAudioTrackMutexHandle, osWaitForever) != osOK) return;

		for (AudioStreamTracker& it : audioStreamTrackers) {
			if (it.audioFileIndex > (int16_t)importedAudioFiles.size() ||
				it.audioFileIndex < 0 || !it.playRequested) continue;
			ImportedAudioFile fil = importedAudioFiles.at(it.audioFileIndex);
			// Save the current active sample location - this can change in the timer interrupt
			uint8_t* activeSampleAddr = it.activeSample;

			// Check for any new needed dma requests - this does not care
			// If the active sample is closer to the filled pointer than 2/3 the buffer size, get more samples to be safe
			// The internal buffer is "circular", so account for the overflow conditions
			while ((activeSampleAddr < it.nextInternalSampleToFill ?
				(uint32_t)(it.nextInternalSampleToFill - activeSampleAddr) :
				InternalSingleBufferBytes + (uint32_t)(it.nextInternalSampleToFill- activeSampleAddr))
				<= (InternalSingleBufferBytes / 3 * 2)) {
				// DMA transfer size is the minimum of:
				// samples to active sample pointer (if active sample pointer > nextSampleToFill pointer)
				// samples to end of file
				// samples to end of internal buffer
				bytesOfBufferLeft = it.internalBuffer + InternalSingleBufferBytes - it.nextInternalSampleToFill;
				bytesOfFileLeft = fil.externalDataAddress + fil.dataSizeBytes - it.nextExternalSampleToRead;
				if (activeSampleAddr > it.nextExternalSampleToRead) {
					bytesUntilActiveSample = activeSampleAddr - it.nextExternalSampleToRead;
				} else {
					bytesUntilActiveSample = bytesOfBufferLeft;
				}
				// Add the DMA request
				req.transferToAddress = it.nextInternalSampleToFill;
				req.transferFromAddress = it.nextExternalSampleToRead;
				req.transferSizeBytes = min(min(bytesOfBufferLeft, bytesOfFileLeft), bytesUntilActiveSample);
				dmaRequestQueue.push(req);
				// Update the pointers to the next external and internal buffer addresses
				it.nextInternalSampleToFill += req.transferSizeBytes;
				if (it.nextInternalSampleToFill == it.internalBuffer + InternalSingleBufferBytes) {
					it.nextInternalSampleToFill = it.internalBuffer;
				}
				it.nextExternalSampleToRead += req.transferSizeBytes;
				if (it.nextExternalSampleToRead == fil.externalDataAddress + fil.dataSizeBytes) {
					it.nextExternalSampleToRead = fil.externalDataAddress;
				}
			}

			// Kick off a dma transfer if there are requests but none are active
			if (!dmaRequestQueue.empty() && !dmaTransferActive) {
				dmaTransferActive = 1;
				req = dmaRequestQueue.front();
				HAL_SDRAM_Read_DMA(&hsdram1, (uint32_t*)req.transferFromAddress,
					(uint32_t*)req.transferToAddress, req.transferSizeBytes);
			}
		}

		// Release mutex for audioStreamTrackerss
		osMutexRelease(spkAudioTrackMutexHandle);
	}
}

void SparkboxAudioManager::mixAudioSample(uint16_t& leftSample, uint16_t& rightSample)
{
	// Calculate the newest sample
	uint16_t temp = 0;
	leftSample = 0;
	rightSample = 0;
	for (AudioStreamTracker& it : audioStreamTrackers) {
		ImportedAudioFile fil = importedAudioFiles.at(it.audioFileIndex);
		// Do not add this stream's audio sample if it is not playing
		if (!it.isPlaying) continue;

		// Account for the various accepted data formats by converting
		// all to 16 bit stereo so they can all be added together
		if (fil.bytesPerSample == 16 && fil.numberOfChannels == 2) {
			// Stereo 16 bit
			leftSample += *((uint16_t*)it.activeSample);
			rightSample += *((uint16_t*)it.activeSample + 1);
		} else if (fil.bytesPerSample == 16 && fil.numberOfChannels == 2) {
			// Stereo 8 bit
			temp = (*(uint16_t*)it.activeSample);
			leftSample += (temp & 0xFFFF0000);
			rightSample += ((temp << 8) & 0xFFFF0000);
		} else if (fil.bytesPerSample == 16 && fil.numberOfChannels == 2) {
			// Mono 16 bit
			leftSample += *((uint16_t*)it.activeSample);
			rightSample += *((uint16_t*)it.activeSample);
		} else {
			// Mono 8 bit
			temp = (*(uint8_t*)it.activeSample);
			leftSample += (temp << 8);
			rightSample += (temp << 8);
		}
	}
}
