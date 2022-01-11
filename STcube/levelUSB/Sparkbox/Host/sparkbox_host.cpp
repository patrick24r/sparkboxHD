#include "sparkbox_host.hpp"

SparkboxLevel* spkLevel;

// Audio declarations
static sparkboxError_t audioInitialize_lowLevel(void);
static sparkboxError_t audioWriteSample_lowLevel(unsigned int newSample);
static sparkboxError_t audioBeginDMATx_lowLevel(void* txFromAddress, void* txToAddress, unsigned int sizeBytes);
static void audioDeinitialize_lowLevel(void);

// These callbacks need to use C conventions to play nice with the STM32H7 HAL library
// and the freeRTOS libraries
extern "C" {
	static void sparkboxAudio_TimerITCallback(TIM_HandleTypeDef* timHandle);
	static void sparkboxAudio_DMACompleteCallback(MDMA_HandleTypeDef* mdmaHandle);
	static void sparkboxAudio_MainTask(void* arg);
}

static const SparkboxAudioDriver_TypeDef audioDriver = {
	.allSamplesBuffer = (unsigned char*)0xC0000000, //< Starting address of external buffer
	.hostInitialize = audioInitialize_lowLevel,
	.hostWriteSample = audioWriteSample_lowLevel,
	.hostBeginDMATx = audioBeginDMATx_lowLevel,
	.hostDeinitialize = audioDeinitialize_lowLevel
};

osThreadId_t audioThreadHandle;
static const osThreadAttr_t audioThreadAttributes = {
	.name = "audioTask",
	.stack_size = 1024 * 4,
	.priority = (osPriority_t)osPriorityNormal,
};

// Video declarations
static sparkboxError_t videoInitialize_lowLevel(void);
static sparkboxError_t videoBeginDMATx_lowLevel(void* txFromAddress, void* txToAddress, unsigned int sizeBytes);
static void videoDeinitialize_lowLevel(void);

// These callbacks need to use C conventions to play nice with the STM32H7 HAL library
// and the freeRTOS libraries
extern "C" {
	static void sparkboxVideo_DMACompleteCallback(MDMA_HandleTypeDef* mdmaHandle);
	static void sparkboxVideo_MainTask(void* arg);
}

static const SparkboxVideoDriver_TypeDef videoDriver = {
	.gpuStartAddress = (unsigned char*)0x60000000,
	.hostInitialize = videoInitialize_lowLevel,
	.hostBeginDMATx = videoBeginDMATx_lowLevel,
	.hostDeinitialize = videoDeinitialize_lowLevel
};

osThreadId_t videoThreadHandle;
static const osThreadAttr_t videoThreadAttributes = {
	.name = "videoTask",
	.stack_size = 512 * 4,
	.priority = (osPriority_t)osPriorityNormal,
};


// Initialization
sparkboxError_t sparkboxLevelInit(string levelDirectory)
{
	sparkboxError_t status;

	// Instantiate everything and catch bad allocation
	try {
		spkLevel = new SparkboxLevel();
	} catch (bad_alloc& ex) {
		return SparkboxError::INIT_ALLOC_FAILED;
	}

	// Link the audio driver
	status = spkLevel->audioMgr->linkDriver(&audioDriver);
	if (status != SparkboxError::SPARK_OK) {
		return status;
	}
	// Initialize audio manager
	status = spkLevel->audioMgr->initialize();
	if (status != SparkboxError::SPARK_OK) {
		return status;
	}

	// Link the video driver
	status = spkLevel->videoMgr->linkDriver(&videoDriver);
	if (status != SparkboxError::SPARK_OK) {
		return status;
	}
	// Initialize video manager
	status = spkLevel->videoMgr->initialize();
	if (status != SparkboxError::SPARK_OK) {
		return status;
	}

	// Import the level
	status = spkLevel->importLevel(levelDirectory);
	if (status != SparkboxError::SPARK_OK) {
		return status;
	}

	return SparkboxError::SPARK_OK;
}

/******************************** Audio driver host/low level functions *************************/
static sparkboxError_t audioInitialize_lowLevel(void)
{
	// Link the mdma to the sdram
	hsdram1.hmdma = &hmdma_mdma_channel40_sw_0;

	// Register the correct callbacks
	HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, sparkboxAudio_TimerITCallback);
	HAL_MDMA_RegisterCallback(&hmdma_mdma_channel40_sw_0, HAL_MDMA_XFER_CPLT_CB_ID, sparkboxAudio_DMACompleteCallback);

	// Start the audio output by repeatedly sending whatever is in the activeSamples variable
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);

	// Start the main audio thread and the audio sample timer
	audioThreadHandle = osThreadNew(sparkboxAudio_MainTask, NULL, &audioThreadAttributes);
	HAL_TIM_Base_Start_IT(&htim7);
	return SparkboxError::SPARK_OK;
}

static sparkboxError_t audioWriteSample_lowLevel(unsigned int newSample)
{
	uint16_t newSampleLeft = (uint16_t)((newSample >> 16) & 0x0000FFFF);
	uint16_t newSampleRight = (uint16_t)(newSample & 0x0000FFFF);
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_L, newSampleLeft);
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_L, newSampleRight);
	return SparkboxError::SPARK_OK;
}

static sparkboxError_t audioBeginDMATx_lowLevel(void* txFromAddress, void* txToAddress, unsigned int sizeBytes)
{
	// Read sample data from sdram into internal SRAM
	HAL_StatusTypeDef status = HAL_SDRAM_Read_DMA(&hsdram1, (uint32_t*)txFromAddress, 
		(uint32_t*)txToAddress, sizeBytes);

	if (status != HAL_OK) {
		return SparkboxError::AUDIO_DMA_TRANSFER_FAILED;
	}

	return SparkboxError::SPARK_OK;
}

static void audioDeinitialize_lowLevel(void)
{
	HAL_TIM_Base_Stop_IT(&htim7);
	osThreadTerminate(audioThreadHandle);
	HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1);
	HAL_DAC_Stop(&hdac1, DAC_CHANNEL_2);
}

static void sparkboxAudio_TimerITCallback(TIM_HandleTypeDef* timHandle)
{
	spkLevel->audioMgr->timerInterruptCallback();
}

static void sparkboxAudio_DMACompleteCallback(MDMA_HandleTypeDef* mdmaHandle)
{
	spkLevel->audioMgr->DMATransferCompleteCallback();
}

static void sparkboxAudio_MainTask(void* arg)
{
	spkLevel->audioMgr->audioThreadFunction();
}


/******************************** Video driver host/low level functions *************************/
static sparkboxError_t videoInitialize_lowLevel(void)
{
	// Link the mdma to the "sram" which is really the gpu
	hsram1.hmdma = &hmdma_mdma_channel41_sw_0;

	// Register the dma complete callback
	HAL_MDMA_RegisterCallback(&hmdma_mdma_channel41_sw_0, HAL_MDMA_XFER_CPLT_CB_ID, sparkboxVideo_DMACompleteCallback);

	videoThreadHandle = osThreadNew(sparkboxVideo_MainTask, NULL, &videoThreadAttributes);
	return SparkboxError::SPARK_OK;
}

static sparkboxError_t videoBeginDMATx_lowLevel(void* txFromAddress, void* txToAddress, unsigned int sizeBytes)
{
	// Write data from internal SRAM to "external SRAM"/GPU
	HAL_SRAM_Write_DMA(&hsram1, (uint32_t*)txToAddress, (uint32_t*)txFromAddress, sizeBytes);
	return SparkboxError::SPARK_OK;
}

static void videoDeinitialize_lowLevel(void)
{
	osThreadTerminate(videoThreadHandle);
}


static void sparkboxVideo_DMACompleteCallback(MDMA_HandleTypeDef* mdmaHandle)
{
	spkLevel->videoMgr->DMATransferCompleteCallback();
}

static void sparkboxVideo_MainTask(void* arg)
{
	spkLevel->videoMgr->videoThreadFunction();
}