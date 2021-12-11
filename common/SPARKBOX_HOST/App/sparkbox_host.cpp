#include "sparkbox_host.hpp"

SparkboxLevel* spkLevel;

// Audio declarations

// These callbacks need to use C conventions to play nice with the STM32H7 HAL library
// and the freeRTOS libraries
extern "C" {
	static void sparkboxAudio_TimerITCallback(TIM_HandleTypeDef* timHandle);
	static void sparkboxAudio_DMACompleteCallback(MDMA_HandleTypeDef* mdmaHandle);
	static void sparkboxAudio_MainTask(void * arg);
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




// Initialization
sparkboxError_t SparkboxLevelInit(string& levelDirectory)
{
	sparkboxError_t status;

	// Instantiate everything
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
	

	// Link the video driver


	// Import the level
	status = spkLevel->importLevel(levelDirectory);
	if (status != SparkboxError::SPARK_OK) {
		return status;
	}

	return SparkboxError::SPARK_OK;
}

/* Audio driver low level functions */
sparkboxError_t audioInitialize_lowLevel(void)
{
	// Initialize audio peripherals
	MX_GPIO_Init();
	MX_FATFS_Init();
	MX_MDMA_Init();
	MX_FMC_Init();
	MX_DAC1_Init();
	MX_TIM7_Init();

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

sparkboxError_t audioWriteSample_lowLevel(unsigned int newSample)
{
	uint16_t newSampleLeft = (uint16_t)((newSample >> 16) & 0x0000FFFF);
	uint16_t newSampleRight = (uint16_t)(newSample & 0x0000FFFF);
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_L, newSampleLeft);
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_L, newSampleRight);
	return SparkboxError::SPARK_OK;
}

sparkboxError_t audioBeginDMATx_lowLevel(void* txFromAddress, void* txToAddress, unsigned int sizeBytes)
{
	HAL_StatusTypeDef status = HAL_SDRAM_Read_DMA(&hsdram1, (uint32_t*)txFromAddress, 
		(uint32_t*)txToAddress, sizeBytes);

	if (status != HAL_OK) {
		return SparkboxError::AUDIO_DMA_TRANSFER_FAILED;
	}

	return SparkboxError::SPARK_OK;
}

void audioDeinitialize_lowLevel(void)
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

static void sparkboxAudio_MainTask(void * arg)
{
	spkLevel->audioMgr->audioThreadFunction();
}


/* Video driver low level functions */