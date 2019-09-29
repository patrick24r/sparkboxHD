#include "SparkboxBIOS.h"

// Constructor
void SparkboxBIOS::SparkboxBIOS()
{
	// Initialize state variables
	// No current valid game number
	currentGameNum = -1;
	// No current valid level number loaded into flash
	loadedLevelNum = -1;

  // Initialize low level peripherals
  lowLevelInit();

  // Initialize GPU
  gpu = new SparkboxGpu();
}

// Destructor
void SparkboxBIOS::~SparkboxBIOS()
{
  delete gpu;
}

// Prompts user to select the game on SD card
int32_t SparkboxBIOS::userSelectGame(void)
{
	currentGameNum = -1;
	// Find all game folders on SD card

	// Display options to user and let them pick

	return currentGameNum;
}

// Sets the game number of the game currently being played
void SparkboxBIOS::setGame(int32_t gameNum)
{
	currentGameNum = gameNum;
}

// Gets the number of the game being played
int32_t SparkboxBIOS::getGame(void)
{
	return currentGameNum;
}

// Helper function to validate a level for current game
bootError_t SparkboxBIOS::validLevel(int32_t level)
{
	return validLevel(level, currentGameNum);
}

// Validate a level for any game
// Returns 0 for valid level
// Returns other error code for invalid level
bootError_t SparkboxBIOS::validLevel(int32_t level, int32_t gameNum)
{
	bootError_t error = NO_ERROR;

	// Check if game folder exists

	// Check if level folder exists in game folder

	// Check for correctly named level file

	// Check if the level code can fit in designated flash memory
	// if (size < MAX_LEVEL_SIZE && size > 0)

	return error;
}

// Load a level's data into memory
// Returns 0 on success
// Returns -1 on failure
bootError_t SparkboxBIOS::loadLevel(int32_t level)
{
	bootError_t error = NO_ERROR;

	// Get list of game folders and pick one specified by currentGameNum

	// Find level files if they exist in currentGameNum's directory

	// Load it into flash memory

	// Return success or failure of flash memory write

	if (error) loadedLevelNum = -1;
	else loadedLevelNum = level;

	return error;

}

// Play a level with previously loaded level
int32_t SparkboxBIOS::playLevel(int32_t level)
{
	// If the code in flash is valid, execute it
	if (loadedLevelNum >= 0 && loadedLevelNum == level) {
		// Call level with peripheral handles
		// Return the next level number

		// Cast LEVEL_START as a function pointer and call it
		return ((int32_t(*))LEVEL_START)();
	}
}

/**
  * @brief Initialize mcu peripherals
  * @retval None
  */
void SparkboxBIOS::lowLevelInit(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  // Initialize the chip
  HAL_Init();

  /** Configure the main internal regulator output voltage */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  /** Initializes the CPU, AHB and APB busses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  /** Initialization of I2S clock */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  // Initialize internal peripherals (Timers, DMA, etc.)
  MX_GPIO_Init();
  MX_I2S2_Init();
  MX_RNG_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
}
