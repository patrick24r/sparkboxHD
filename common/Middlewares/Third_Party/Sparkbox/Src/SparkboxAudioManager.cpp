#include "SparkboxAudioManager.hpp"

#define WAV_HEADER_SIZE (44UL)
#define WAV_HEADER_NUMCHANNELS_POS (22UL)
#define WAV_HEADER_SAMPLERATE_POS (24UL)
#define WAV_HEADER_BLOCKALIGN_POS (32UL)
#define WAV_HEADER_BITSPERSAMPLE_POS (34UL)
#define WAVE_HEADER_DATASIZE_POS (40UL)

SparkboxAudioManager::SparkboxAudioManager()
{
	// Reserve memory for internal audio buffers and trackers
	importedAudioFiles.reserve(MAX_AUDIO_FILES);
	audioStreamTrackers.reserve(MAX_AUDIO_STREAMS);

	// Add blank trackers to start with
	for (int i = 0; i < MAX_AUDIO_STREAMS; i++) {
		audioStreamTrackers.push_back(AudioStreamTracker());
	}
}

SparkboxAudioManager::~SparkboxAudioManager()
{
	// Deinitialize the host if it is initialized
	if (isInitialized && driver != NULL) {
		driver->hostDeinitialize();
	}
}

sparkboxError_t SparkboxAudioManager::linkDriver(const SparkboxAudioDriver_TypeDef* audioDriver)
{
	// Check that the driver is populated before assigning it
	if (audioDriver == NULL || 
		audioDriver->allSamplesBuffer == NULL ||
		audioDriver->hostInitialize == NULL ||
		audioDriver->hostWriteSample == NULL ||
		audioDriver->hostBeginDMATx == NULL ||
		audioDriver->hostDeinitialize == NULL) {
		return SparkboxError::AUDIO_DRIVER_STRUCT_INVALID;
	} else {
		driver = audioDriver;
		isInitialized = 0; // New driver, low level is no longer initialized
		return SparkboxError::SPARK_OK;
	}
}

sparkboxError_t SparkboxAudioManager::initialize()
{
	sparkboxError_t status;
	if (driver == NULL) {
		return SparkboxError::AUDIO_DRIVER_STRUCT_INVALID;
	} else if (isInitialized) {
		return SparkboxError::SPARK_OK;
	}

	status = driver->hostInitialize();
    if (status == SparkboxError::SPARK_OK) isInitialized = 1;
	return status;
}

/*
 *
 */
sparkboxError_t SparkboxAudioManager::importAllAudioFiles(string directoryPath)
{
	DIR dir;
	FRESULT res;
	FILINFO fno;
	sparkboxError_t addFileResult;
	string filePath, fileExt;

	if (!isInitialized) return SparkboxError::AUDIO_NOT_INITIALIZED;

	// Check if the folder exists
	res = f_stat(directoryPath.c_str(), &fno);
	if (res != FR_OK) return SparkboxError::AUDIO_FATFS_ERROR;
	if (!(fno.fattrib & AM_DIR)) return SparkboxError::AUDIO_NOT_A_DIRECTORY;

	// Check through the folder for valid file extensions
	res = f_opendir(&dir, directoryPath.c_str());
	if (res != FR_OK) return SparkboxError::AUDIO_FATFS_ERROR;

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
	return SparkboxError::SPARK_OK;
}

/*
 * @brief
 */
sparkboxError_t SparkboxAudioManager::importAudioFile(string filePath)
{
	if (!isInitialized) return SparkboxError::AUDIO_NOT_INITIALIZED;

	// Check that this file hasn't already been imported
	for (auto& file : importedAudioFiles) {
		if (file.importedFilePath == filePath) return SparkboxError::AUDIO_FILE_ALREADY_IMPORTED;
	}

	// Check file extension

	// return importWAVFile(filePath);
	return SparkboxError::SPARK_OK;
}

sparkboxError_t SparkboxAudioManager::importWAVFile(string filePath)
{
	ImportedAudioFile fileImport;
	FIL fileRead;
	FILINFO fno;
	FRESULT fRes;
	uint8_t tempBuffer[_MAX_SS];
	UINT bytesRead;
	uint32_t bytesReadOffset = 0;

	// Check that the file exists and is supported
	fRes = f_stat(filePath.c_str(), &fno);
	if (fRes != FR_OK) return SparkboxError::AUDIO_FATFS_ERROR;
	fRes = f_open(&fileRead, filePath.c_str(), FA_READ);
	if (fRes != FR_OK) return SparkboxError::AUDIO_FATFS_ERROR;

	// Check that the file can fit in memory (after decompression)
	if (f_size(&fileRead) < WAV_HEADER_SIZE ||
		f_size(&fileRead) > (ALL_SAMPLES_BUFFER_SIZE_BYTES - totalAudioBytesImported)) {
		f_close(&fileRead);
		return SparkboxError::AUDIO_FILE_INSUFFICIENT_MEMORY;
	}

	// Read the file header for initial WAV data
	fRes = f_read(&fileRead, tempBuffer, WAV_HEADER_SIZE, &bytesRead);
	if (fRes != FR_OK || bytesRead != WAV_HEADER_SIZE) {
		f_close(&fileRead);
		return SparkboxError::AUDIO_FATFS_ERROR;
	}

	// Check for wave file IDs to verify this is a WAV file
	if (tempBuffer[0] != 'R' || tempBuffer[1] != 'I' || tempBuffer[2] != 'F' || tempBuffer[3] != 'F' ||
		tempBuffer[8] != 'W' || tempBuffer[9] != 'A' || tempBuffer[10] != 'V' || tempBuffer[11] != 'E' ||
		tempBuffer[12] != 'f' || tempBuffer[13] != 'm' || tempBuffer[14] != 't' || tempBuffer[15] != 0x20 ||
		tempBuffer[36] != 'd' || tempBuffer[37] != 'a' || tempBuffer[38] != 't' || tempBuffer[39] != 'a') {
		f_close(&fileRead);
		return SparkboxError::AUDIO_FILE_INVALID_HEADER;
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
		return SparkboxError::AUDIO_FILE_INVALID_HEADER;
	}

	fileImport.numberOfSamples = fileImport.dataSizeBytes / fileImport.blockAlign;
	fileImport.importedFilePath = filePath;

	// Append this file's sample data to the end of the previous file's. If this is the
	// first file, begin at where the driver says the data starts
	if (importedAudioFiles.empty()) {
		fileImport.externalDataAddress = (unsigned char*)driver->allSamplesBuffer;
	}
	else {
		fileImport.externalDataAddress = importedAudioFiles.back().externalDataAddress +
			importedAudioFiles.back().dataSizeBytes;
	}
	fileImport.dataSizeBytes = fileImport.numberOfSamples * fileImport.blockAlign;

	// Read the samples into memory
	do {
		fRes = f_read(&fileRead, tempBuffer, _MAX_SS, &bytesRead);
		if (fRes != FR_OK) {
			f_close(&fileRead);
			return SparkboxError::AUDIO_FATFS_ERROR;
		}

		// TODO: Convert whatever form the data is in to unsigned 16 bit for the DAC

		memcpy(fileImport.externalDataAddress + bytesReadOffset, tempBuffer, bytesRead);
		bytesReadOffset += bytesRead;
	} while (bytesRead == _MAX_SS);
	f_close(&fileRead);

	// After adding it successfully, add to the list of imported files
	importedAudioFiles.push_back(fileImport);

	return SparkboxError::SPARK_OK;
}

/*
 * @brief
 */
sparkboxError_t SparkboxAudioManager::setAudioStream(unsigned char audioStream, string audioFilePath)
{
	int separatorIdx;
	string fullPath, fileName;
	// Try to match the audio file path
	for (auto it = importedAudioFiles.begin(); it != importedAudioFiles.end(); ++it)
	{
		// Isolate the file name from the full imported path
		fullPath = it->importedFilePath;
		separatorIdx = fullPath.find_last_of("/\\");
		if ((unsigned int)separatorIdx < fullPath.length()) {
			fileName = fullPath.substr(separatorIdx + 1);
		} else {
			fileName = fullPath.substr(0);
		}

		// Check if the requested file name matches an imported one 
		if (audioFilePath == fullPath || audioFilePath == fileName) {
			return setAudioStream(audioStream, (uint8_t)(it - importedAudioFiles.begin()));
		}
	}

	return SparkboxError::AUDIO_FILE_NOT_IMPORTED;
}

/*
 * @brief
 * @param audioStream
 * @param audioFileID
 */
sparkboxError_t SparkboxAudioManager::setAudioStream(unsigned char audioStream, unsigned int audioFileID)
{
	// Reset the audio stream
	sparkboxError_t status = resetAudioStream(audioStream);
	if (status != SparkboxError::SPARK_OK) return status;

	while (streamsLocked);
	streamsLocked = 1;

	// Set the audio stream to the correct file
	audioStreamTrackers.at(audioStream).audioFileIndex = audioFileID;
	audioStreamTrackers.at(audioStream).nextSampleToRead = importedAudioFiles.at(audioFileID).externalDataAddress;

	streamsLocked = 0;
	return SparkboxError::SPARK_OK;
}

/*
 *
 */
sparkboxError_t SparkboxAudioManager::resetAudioStream(unsigned char audioStream)
{
	if (!isInitialized) return SparkboxError::AUDIO_NOT_INITIALIZED;
	if (audioStream > MAX_AUDIO_STREAMS) return SparkboxError::AUDIO_STREAM_OUT_OF_RANGE;
	while (streamsLocked);
	streamsLocked = 1;

	AudioStreamTracker* tracker = &audioStreamTrackers.at(audioStream);
	// Safely stop any audio currently playing
	tracker->playRequested = 0;
	tracker->isPlaying = 0;
	tracker->audioFileIndex = -1;
	// Reset all tracker values and internal buffer
	tracker->playsRemaining = 0;
	tracker->samplesRemaining = 0;
	tracker->sampleRateDivisor = 1;
	tracker->sampleRateCounter = 0;
	tracker->nextSampleToRead = NULL;
	tracker->activeSample = tracker->internalBuffer;
	tracker->nextInternalSampleToFill = tracker->internalBuffer;
	memset(tracker->internalBuffer, 0, STREAM_BUFFER_SIZE_BYTES);

	streamsLocked = 0;
	return SparkboxError::SPARK_OK;
}

sparkboxError_t SparkboxAudioManager::playAudioStream(unsigned char audioStream, int numberOfPlays)
{
	if (!isInitialized) return SparkboxError::AUDIO_NOT_INITIALIZED;
	if (audioStream > MAX_AUDIO_STREAMS) return SparkboxError::AUDIO_STREAM_OUT_OF_RANGE;
	resetAudioStream(audioStream);
	audioStreamTrackers.at(audioStream).playsRemaining = numberOfPlays;
	resumeAudioStream(audioStream);
	return SparkboxError::SPARK_OK;
}

sparkboxError_t SparkboxAudioManager::stopAudioStream(unsigned char audioStream)
{
	if (!isInitialized) return SparkboxError::AUDIO_NOT_INITIALIZED;
	if (audioStream > MAX_AUDIO_STREAMS) return SparkboxError::AUDIO_STREAM_OUT_OF_RANGE;
	while (streamsLocked);
	streamsLocked = 1;



	streamsLocked = 0;
	return SparkboxError::SPARK_OK;
}

sparkboxError_t SparkboxAudioManager::resumeAudioStream(unsigned char audioStream)
{
	if (!isInitialized) return SparkboxError::AUDIO_NOT_INITIALIZED;
	if (audioStream > MAX_AUDIO_STREAMS) return SparkboxError::AUDIO_STREAM_OUT_OF_RANGE;
	while (streamsLocked);
	streamsLocked = 1;

	// Request the stream to begin playing
	audioStreamTrackers.at(audioStream).playRequested = 1;

	streamsLocked = 0;
	return SparkboxError::SPARK_OK;
}

sparkboxError_t SparkboxAudioManager::rewindAudioStream(unsigned char audioStream, float numberOfSeconds)
{
	if (!isInitialized) return SparkboxError::AUDIO_NOT_INITIALIZED;
	if (audioStream > MAX_AUDIO_STREAMS) return SparkboxError::AUDIO_STREAM_OUT_OF_RANGE;
	while (streamsLocked);
	streamsLocked = 1;



	streamsLocked = 0;
	return SparkboxError::SPARK_OK;
}

sparkboxError_t SparkboxAudioManager::rewindAudioStream(unsigned char audioStream, int numberOfSamples)
{
	if (!isInitialized) return SparkboxError::AUDIO_NOT_INITIALIZED;
	if (audioStream > MAX_AUDIO_STREAMS) return SparkboxError::AUDIO_STREAM_OUT_OF_RANGE;
	while (streamsLocked);
	streamsLocked = 1;



	streamsLocked = 0;
	return SparkboxError::SPARK_OK;
}

sparkboxError_t SparkboxAudioManager::skipAudioStream(unsigned char audioStream, float numberOfSeconds)
{
	if (!isInitialized) return SparkboxError::AUDIO_NOT_INITIALIZED;
	if (audioStream > MAX_AUDIO_STREAMS) return SparkboxError::AUDIO_STREAM_OUT_OF_RANGE;
	while (streamsLocked);
	streamsLocked = 1;



	streamsLocked = 0;
	return SparkboxError::SPARK_OK;
}

sparkboxError_t SparkboxAudioManager::skipAudioStream(unsigned char audioStream, int numberOfSamples)
{
	if (!isInitialized) return SparkboxError::AUDIO_NOT_INITIALIZED;
	if (audioStream > MAX_AUDIO_STREAMS) return SparkboxError::AUDIO_STREAM_OUT_OF_RANGE;
	while (streamsLocked);
	streamsLocked = 1;



	streamsLocked = 0;
	return SparkboxError::SPARK_OK;
}


// DMA transfer from external buffer to internal buffer is complete
void SparkboxAudioManager::DMATransferCompleteCallback(void)
{
	AudioDmaRequest nextDmaTx;

	// Do nothing if not initialized
	if (!isInitialized || dmaRequestQueue.size() == 0) return;

	// Pop the request that just finished
	dmaRequestQueue.pop();
	// Begin the next dma transfer if one exists
	if (!dmaRequestQueue.empty()) {
		nextDmaTx = dmaRequestQueue.front();
		driver->hostBeginDMATx(nextDmaTx.transferFromAddress, 
			nextDmaTx.transferToAddress, nextDmaTx.transferSizeBytes);
	} else {
		dmaTransferActive = 0;
	}
}

// Timer interrupt callback

void SparkboxAudioManager::timerInterruptCallback(void)
{
	int mixedSample;
	return; // TODO: Actually mix the sample

	// Do nothing if not initialized
	if (!isInitialized) return;

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
			if (it.activeSample >= it.internalBuffer + STREAM_BUFFER_SIZE_BYTES) {
				it.activeSample = it.internalBuffer;
			}

			// If there is a sample overrun, stop playing
			if (it.activeSample == it.nextInternalSampleToFill) {
				it.isPlaying = 0;
			} else if (it.playRequested && !it.isPlaying) {
				// If there is a 2/3 or less buffer ready ahead of the active sample, begin playing
				if (it.activeSample < it.nextInternalSampleToFill) {
					it.isPlaying = (uint32_t)(it.nextInternalSampleToFill - it.activeSample) >= STREAM_BUFFER_SIZE_BYTES / 2;
				} else {
					it.isPlaying = (STREAM_BUFFER_SIZE_BYTES + (uint32_t)(it.nextInternalSampleToFill - it.activeSample))
						>= STREAM_BUFFER_SIZE_BYTES / 2;
				}
			}
		}
	}
	// Write newest sample value to DACs
	mixedSample = mixAudioSample();
	driver->hostWriteSample(mixedSample);
}

// Main thread function
// For each audio stream
// - Check for any required dma requests
// - Check if the dma requests need to be "kicked off"
// - Check if any streams the user wants to play can be played
void SparkboxAudioManager::audioThreadFunction()
{
	while(1) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		osDelay(250);
	}

	// Do nothing until initialized
	while (!isInitialized) {
		osDelay(100);
	}

	// Repeat forever
	while (1) {
		do {
			// Wait at least 1 ms between updates until the audio streams are unlocked
			osDelay(1); 
		} while (streamsLocked);
		streamsLocked = 1;

		// Check if any streams require a DMA request

		// unlock the audio stream data
		streamsLocked = 0;
	}
}

/*
 * @brief
 * @param
 * @return mixed audio sample
 */
int SparkboxAudioManager::mixAudioSample()
{
	// Calculate the newest sample
	return 0;
}
