#include "SparkboxAudioManager.h"

#define WAV_HEADER_SIZE (44UL)
#define WAV_HEADER_NUMCHANNELS_POS (22UL)
#define WAV_HEADER_SAMPLERATE_POS (24UL)
#define WAV_HEADER_BLOCKALIGN_POS (32UL)
#define WAV_HEADER_BITSPERSAMPLE_POS (34UL)
#define WAVE_HEADER_DATASIZE_POS (40UL)

void audioThreadWrapper(void* arg)
{
	SparkboxAudioManager::threadfcn_AudioManager(arg);
}

SparkboxAudioManager::SparkboxAudioManager(void)
{
	// Initialize the internal audio buffers and trackers
	for (uint i = 0; i < MaxAudioStreams; i++) {
		audioStreamTrackers.push_back(AudioStreamTracker(InternalSingleBufferBytes));
	}

	// Link the mdma to the sdram
	hsdram1.hmdma = &hmdma_mdma_channel40_sw_0; 

	// Create the mutex
	spkAudioTrackMutexHandle = osMutexNew(&spkAudioTrackMutex_attributes);

	
	HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, sparkboxCallback_audioIT);
	HAL_MDMA_RegisterCallback(&hmdma_mdma_channel40_sw_0, HAL_MDMA_XFER_CPLT_CB_ID, sparkboxCallback_audioDMACplt);
	
	// Start the audio output by repeatedly sending whatever is in the activeSamples variable
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);

	// Start the main audio thread and the audio sample timer
	threadHandle = osThreadNew(audioThreadWrapper, (void*)this, &threadTask_attributes);
	HAL_TIM_Base_Start_IT(&htim7);
}

SparkboxAudioManager::~SparkboxAudioManager()
{
	HAL_TIM_Base_Stop_IT(&htim7);
	osThreadTerminate(threadHandle);
	HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1);
	HAL_DAC_Stop(&hdac1, DAC_CHANNEL_2);
	osMutexDelete(spkAudioTrackMutexHandle);
}


int32_t SparkboxAudioManager::importAllAudioFiles(string directoryPath)
{
	DIR dir;
	FRESULT res;
	FILINFO fno;
	int32_t addFileResult = 0;
	string filePath, fileExt;

	// Check if the folder exists
	res = f_stat(directoryPath.c_str(), &fno);
	if (res != FR_OK) return res;
	if (!(fno.fattrib & AM_DIR)) return -1;

	// Check through the folder for valid file extensions
	res = f_opendir(&dir, directoryPath.c_str());
	if (res != FR_OK) return res;

	while (res == FR_OK) {
		res = f_readdir(&dir, &fno);
		// On error or end of directory, break
		if (res != FR_OK || fno.fname[0] == 0) break;
		// on subdirectory, hidden file, or system file, continue
		if (fno.fattrib & (AM_DIR | AM_HID | AM_SYS)) continue;

		// Go to next file if the file is not a wav or mp3
		filePath.assign(fno.fname);
		if (filePath.size() < 5) continue;
		fileExt = filePath.substr(filePath.length() - 4, 4);
		if (fileExt != ".wav" && fileExt != ".mp3" &&
			fileExt != ".WAV" && fileExt != ".MP3") continue;

		// Add the wav or mp3 file 
		addFileResult = importAudioFile(filePath);

		// Stop on any error
		if (addFileResult) return addFileResult;
	}
	return 0;
}

// Give the user both the return error code and the id number for the file
int32_t SparkboxAudioManager::importAudioFile(string filePath)
{
	ImportedAudioFile fileImport;
	FIL fileRead;
	FILINFO fno;
	FRESULT fRes;
	uint8_t tempBuffer[_MAX_SS];
	UINT bytesRead;
	uint32_t bytesReadOffset = 0;

	// Check that this file hasn't already been imported
	for (auto& file : importedAudioFiles) {
		if (file.importedFilePath == filePath) return -1;
	}
	// Check that the file exists and is supported
	fRes = f_stat(filePath.c_str(), &fno);
	if (fRes != FR_OK) return fRes;
	fRes = f_open(&fileRead, filePath.c_str(), FA_READ);
	if (fRes != FR_OK) return fRes;
	// Check that the file can fit in memory (after decompression)
	if (f_size(&fileRead) < WAV_HEADER_SIZE ||
		f_size(&fileRead) > (ExternalBufferBytes - totalAudioBytesImported)) {
		f_close(&fileRead);
		return -1;
	}
	// Read the file header for initial WAV data
	fRes = f_read(&fileRead, tempBuffer, WAV_HEADER_SIZE, &bytesRead);
	if (fRes != FR_OK || bytesRead != WAV_HEADER_SIZE) {
		f_close(&fileRead);
		return -1;
	}

	// Check for wave file IDs to verify this is a WAV file
	if (tempBuffer[0] != 'R' || tempBuffer[1] != 'I' || tempBuffer[2] != 'F' || tempBuffer[3] != 'F' ||
		tempBuffer[8] != 'W' || tempBuffer[9] != 'A' || tempBuffer[10] != 'V' || tempBuffer[11] != 'E' ||
		tempBuffer[12] != 'f' || tempBuffer[13] != 'm' || tempBuffer[14] != 't' || tempBuffer[15] != 0x20 ||
		tempBuffer[36] != 'd' || tempBuffer[37] != 'a' || tempBuffer[38] != 't' || tempBuffer[39] != 'a') {
		f_close(&fileRead);
		return -1;
	}

	// Populate the ImportedAudioFile fields
	fileImport.numberOfChannels = tempBuffer[WAV_HEADER_NUMCHANNELS_POS];
	fileImport.sampleRate = *(uint32_t*)(tempBuffer + WAV_HEADER_SAMPLERATE_POS);
	fileImport.bytesPerSample = *(uint16_t*)(tempBuffer + WAV_HEADER_BITSPERSAMPLE_POS) >> 3;
	fileImport.blockAlign = *(uint16_t*)(tempBuffer + WAV_HEADER_BLOCKALIGN_POS);
	fileImport.dataSizeBytes = *(uint32_t*)(tempBuffer + WAVE_HEADER_DATASIZE_POS);

	// Sanity check all fields we read to verify file integrity
	if (fileImport.blockAlign != fileImport.bytesPerSample * fileImport.numberOfChannels ||
		(fileImport.numberOfChannels != 1 && fileImport.numberOfChannels != 2) ||
		fileImport.blockAlign == 0 || fileImport.bytesPerSample == 0 || 
		fileImport.sampleRate == 0 || fileImport.dataSizeBytes == 0) {
		f_close(&fileRead);
		return -1;
	}

	fileImport.numberOfSamples = fileImport.dataSizeBytes / fileImport.blockAlign;
	fileImport.importedFilePath = filePath;
	if (importedAudioFiles.empty()) {
		fileImport.externalDataAddress = (uint8_t*)ExternalBufferAddress;
	}
	else {
		fileImport.externalDataAddress = importedAudioFiles.back().externalDataAddress +
			importedAudioFiles.back().dataSizeBytes;
	}
	fileImport.dataSizeBytes = fileImport.numberOfSamples * fileImport.blockAlign;

	// Read the samples into memory
	// TODO: Converty PCM (signed) to unsigned for the DAC
	do {
		fRes = f_read(&fileRead, tempBuffer, _MAX_SS, &bytesRead);
		if (fRes != FR_OK) {
			f_close(&fileRead);
			return -1;
		}


		memcpy(fileImport.externalDataAddress + bytesReadOffset, tempBuffer, bytesRead);
		bytesReadOffset += bytesRead;
	} while (bytesRead == _MAX_SS);
	
	f_close(&fileRead);
	if (fRes != FR_OK || fileImport.dataSizeBytes != bytesRead) return -1;

	// After adding it successfully, add to the list of imported files
	importedAudioFiles.push_back(fileImport);

	return 0;
}

int32_t SparkboxAudioManager::setAudioStream(uint8_t audioStream, string audioFilePath)
{
	// acquire mutex for audioStreamTrackers
	if (osMutexAcquire(spkAudioTrackMutexHandle, osWaitForever) != osOK) return -1;

	// Try to match the audio file path
	for (auto it = importedAudioFiles.begin(); it != importedAudioFiles.end(); ++it)
	{
		if (audioFilePath == it->importedFilePath) {
			// release mutex for audioStreamTrackers
			osMutexRelease(spkAudioTrackMutexHandle);
			return setAudioStream(audioStream, (uint8_t)(it - importedAudioFiles.begin()));
		}
	}
	// release mutex for audioStreamTrackers
	osMutexRelease(spkAudioTrackMutexHandle);
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
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	return;

	// TODO: add a mutex to prevent reentrancy
	uint16_t newSampleLeft = 0, newSampleRight = 0;
	for (AudioStreamTracker& it : audioStreamTrackers) {
		if (it.audioFileIndex >= (int16_t)importedAudioFiles.size() ||
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
void SparkboxAudioManager::threadfcn_AudioManager(void* arg)
{
	SparkboxAudioManager* aud = (SparkboxAudioManager*)arg;
	AudioDmaRequest req;
	uint32_t bytesOfBufferLeft, bytesOfFileLeft, bytesUntilActiveSample;

	while(1) {
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		osDelay(500);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		osDelay(500);
	}

	// Repeat forever
	while (1) {
		osDelay(1); // Wait a 1 ms between updates

		// acquire mutex for audioStreamTrackers
		if (osMutexAcquire(aud->spkAudioTrackMutexHandle, osWaitForever) != osOK) return;

		for (AudioStreamTracker& it : aud->audioStreamTrackers) {
			if (it.audioFileIndex >= (int16_t)aud->importedAudioFiles.size() ||
				it.audioFileIndex < 0 || !it.playRequested) continue;
			ImportedAudioFile fil = aud->importedAudioFiles.at(it.audioFileIndex);
			// Save the current active sample location - this can change in the timer interrupt
			uint8_t* activeSampleAddr = it.activeSample;

			// Check for any new needed dma requests - this does not care
			// If the active sample is closer to the filled pointer than 2/3 the buffer size, get more samples to be safe
			// The internal buffer is "circular", so account for the overflow conditions
			while ((activeSampleAddr < it.nextInternalSampleToFill ?
				(uint32_t)(it.nextInternalSampleToFill - activeSampleAddr) :
				aud->InternalSingleBufferBytes + (uint32_t)(it.nextInternalSampleToFill- activeSampleAddr))
				<= (aud->InternalSingleBufferBytes / 3 * 2)) {
				// DMA transfer size is the minimum of:
				// samples to active sample pointer (if active sample pointer > nextSampleToFill pointer)
				// samples to end of file
				// samples to end of internal buffer
				bytesOfBufferLeft = it.internalBuffer + aud->InternalSingleBufferBytes - it.nextInternalSampleToFill;
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
				aud->dmaRequestQueue.push(req);
				// Update the pointers to the next external and internal buffer addresses
				it.nextInternalSampleToFill += req.transferSizeBytes;
				if (it.nextInternalSampleToFill == it.internalBuffer + aud->InternalSingleBufferBytes) {
					it.nextInternalSampleToFill = it.internalBuffer;
				}
				it.nextExternalSampleToRead += req.transferSizeBytes;
				if (it.nextExternalSampleToRead == fil.externalDataAddress + fil.dataSizeBytes) {
					it.nextExternalSampleToRead = fil.externalDataAddress;
				}
			}
		}

		// Kick off a dma transfer if there are requests but none are active
		if (!aud->dmaRequestQueue.empty() && !aud->dmaTransferActive) {
			aud->dmaTransferActive = 1;
			req = aud->dmaRequestQueue.front();
			HAL_SDRAM_Read_DMA(&hsdram1, (uint32_t*)req.transferFromAddress,
				(uint32_t*)req.transferToAddress, req.transferSizeBytes);
		}

		// Release mutex for audioStreamTrackerss
		osMutexRelease(aud->spkAudioTrackMutexHandle);
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
			// Stereo 16 bit (32 bit block)
			leftSample += *((uint16_t*)it.activeSample);
			rightSample += *((uint16_t*)it.activeSample + 1);
		} else if (fil.bytesPerSample == 16 && fil.numberOfChannels == 2) {
			// Stereo 8 bit (16 bit block)
			temp = (*(uint16_t*)it.activeSample);
			leftSample += (temp & 0xFFFF0000); // Mask out right sample
			rightSample += ((temp & 0x0000FFFF) << 8); // Mask out left sample
		} else if (fil.bytesPerSample == 16 && fil.numberOfChannels == 2) {
			// Mono 16 bit (16 bit block)
			leftSample += *((uint16_t*)it.activeSample);
			rightSample += *((uint16_t*)it.activeSample);
		} else {
			// Mono 8 bit (8 bit block)
			temp = (*(uint8_t*)it.activeSample);
			leftSample += (temp << 8);
			rightSample += (temp << 8);
		}
	}
}
