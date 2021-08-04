#include "SparkboxAudioManager.h"

/* Wrappers for callback functions */
void cb_dmaComplete(void)
{
}

void cb_audioTimerIT(void)
{
}


SparkboxAudioManager::SparkboxAudioManager(void)
{
	// Initialize the internal audio buffers and trackers
	for (uint i = 0; i < MaxAudioStreams; i++) {
		audioPlayerTrackers.push_back(AudioStreamTracker(InternalSingleBufferBytes));
	}

	// Initialize relevant peripherals
	MX_FMC_Init();
	MX_DAC1_Init();
	MX_DMA_Init();
	MX_TIM6_Init();

	// Start the audio output by repeatedly sending whatever is in the activeSamples variable
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
}

SparkboxAudioManager::~SparkboxAudioManager()
{
	HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1);
	HAL_DAC_Stop(&hdac1, DAC_CHANNEL_2);
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
	}
}

// Timer interrupt callback
void SparkboxAudioManager::callback_AudioTimerIT(void)
{
	AudioDmaRequest req;
	uint8_t* txToAddr;
	uint8_t* txLimit;
	uint32_t bytesOfBufferLeft, bytesOfFileLeft;

	for (AudioStreamTracker& it : audioPlayerTrackers) {
		// Check if the audio streams are moving to the next sample
		if (it.isPlaying && ++it.sampleRateCounter == it.sampleRateDivisor) {
			it.sampleRateCounter = 0;
			// Increment the next sample pointer to the next full sample
			it.activeSample += importedAudioFiles.at(it.audioFileIndex).blockAlign;
			it.samplesRemaining--;

			// Check if we played the last of a audio stream and apply the audio repetition logic
			if (it.samplesRemaining == 0) {
				// Reset the number of samples yet to play
				it.samplesRemaining = importedAudioFiles.at(it.audioFileIndex).numberOfSamples;
				// Only decrement repeatsRemaining if it's positive. A negative
				// value indicates repeating forever and we hate integer overflow here
				if (it.repeatsRemaining > 0) it.repeatsRemaining--;
				// Turn off the audio if the
				if (it.repeatsRemaining == 0) it.isPlaying = 0;
			}

			// Return to the start of the internal buffer if the pointer ran past the end
			if (it.activeSample >= it.internalBuffer + InternalSingleBufferBytes) {
				it.activeSample = it.internalBuffer;
			}

			// If we need more data because we just finished a half buffer, add it to the dma request queue
			if (it.activeSample == it.internalBuffer || it.activeSample == it.internalBuffer + InternalSingleBufferBytes / 2) {

				// If the external buffer is smaller than the internal one OR
				// we hit the end of the file, we need multiple transfers
				if (it.activeSample == it.internalBuffer) {
					txToAddr = it.internalBuffer + InternalSingleBufferBytes / 2;
				} else {
					txToAddr = it.internalBuffer;
				}
				txLimit = txToAddr + InternalSingleBufferBytes / 2;
				while (txToAddr < txLimit) {
					// The size of the transfer is the minimum of:
					// - bytes before the end of the audio file
					// - bytes of the buffer yet to be filled
					ImportedAudioFile fil = importedAudioFiles.at(it.audioFileIndex);
					bytesOfFileLeft = fil.externalDataAddress + fil.dataSizeBytes - it.nextExternalSampleToRead;
					bytesOfBufferLeft = txLimit - txToAddr;

					// Change request's values to the proper ones and add it to the queue
					req.transferFromAddress = it.nextExternalSampleToRead;
					req.transferToAddress = txToAddr;
					if (bytesOfBufferLeft <= bytesOfFileLeft) {
						req.transferSizeBytes = bytesOfBufferLeft;
						req.transferFillsBuffer = 1;
					} else {
						req.transferSizeBytes = bytesOfFileLeft;
						req.transferFillsBuffer = 0;
					}
					dmaRequestQueue.push(req);

					// Update the next transfer's memory addresses
					it.nextExternalSampleToRead += req.transferSizeBytes;
					// Check for audio file overflow
					if (it.nextExternalSampleToRead >= fil.externalDataAddress + fil.dataSizeBytes) {
						it.nextExternalSampleToRead = fil.externalDataAddress;
					}
					txToAddr += req.transferSizeBytes;
				}
			}


		}
	}

	// Calculate the newest sample
	uint32_t newSampleLeft = 0, newSampleRight = 0;
	for (AudioStreamTracker& it : audioPlayerTrackers) {
		ImportedAudioFile fil = importedAudioFiles.at(it.audioFileIndex);
		// Account for the various accepted data formats
		if (fil.bytesPerSample == 16 && fil.numberOfChannels == 2) {
			// Stereo 16 bit
			newSampleLeft += *((uint16_t*)it.activeSample);
			newSampleRight += *((uint16_t*)it.activeSample + 1);
		} else if (fil.bytesPerSample == 16 && fil.numberOfChannels == 2) {
			// Stereo 8 bit
			newSampleLeft += ((uint16_t)(*(uint8_t*)it.activeSample) << 8);
			newSampleRight+= ((uint16_t)(*((uint8_t*)it.activeSample + 1)) << 8);
		}
		else if (fil.bytesPerSample == 16 && fil.numberOfChannels == 2) {
			// Mono 16 bit
			newSampleLeft += *((uint16_t*)it.activeSample);
			newSampleRight += *((uint16_t*)it.activeSample);
		} else {
			// Mono 8 bit
			newSampleLeft += ((uint16_t)(*(uint8_t*)it.activeSample) << 8);
			newSampleRight += ((uint16_t)(*(uint8_t*)it.activeSample) << 8);
		}
	}
	// Write to DACs
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_L, newSampleLeft);
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_L, newSampleRight);

}
