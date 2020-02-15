#include "Sparkbox.h"

Sparkbox::Sparkbox()
{
  // Initialize low level peripherals
  lowLevelInit();

  // Mount the file system, ready to open and read files
  f_mount(&SDFatFs, (TCHAR const*)SDPath, 0);

  // TODO: Have input arguments define pins maybe?
  // That would make this WAY more flexible
  video = SparkboxVideo();
  audio = SparkboxAudio();
  controller = SparkboxController();
}

// Delete all Sparkbox allocated memory
Sparkbox::~Sparkbox()
{
  // Call audio, video, controller destructors
  controller.~SparkboxController();
  audio.~SparkboxAudio();
  video.~SparkboxVideo();
  
  // Unmount file system
  f_mount(0, SDPath, 0);

  // Free all memory that was allocated
  lowLevelDeInit();
}

// Import sprites in rootDirectory into cpu and gpu memory
int32_t Sparkbox::importLevel(std::string directory)
{
  gpu.addAllSprites(directory);
  audio.addAllAudio(directory);
}

uint32_t Sparkbox::rand(void)
{
  uint32_t rng = 0;
  HAL_RNG_GenerateRandomNumber(&hrng, &rng);
  return rng;
}

void lowLevelInit(void)
{
  // Low level handles ------------------------------------------------------
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  FSMC_NAND_PCC_TimingTypeDef ComSpaceTiming = {0};
  FSMC_NAND_PCC_TimingTypeDef AttSpaceTiming = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Initialize clock and HAL functions -------------------------------------
  HAL_Init();

  // Initialize clock -------------------------------------------------------
  /* Configure the main internal regulator output voltage */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  // Initialize LEDs ------------------------------------------------------
  HAL_GPIO_WritePin(GPIOB, LED0_Pin|LED1_Pin|LED2_Pin|LED3_Pin, GPIO_PIN_RESET);
  /* Configure GPIO pins : LED0_Pin LED1_Pin LED2_Pin LED3_Pin */
  GPIO_InitStruct.Pin = LED0_Pin|LED1_Pin|LED2_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Initialize NAND flash -------------------------------------------------
  /* Perform the NAND1 memory initialization sequence */
  hnand1.Instance = FSMC_NAND_DEVICE;
  /* hnand1.Init */
  hnand1.Init.NandBank = FSMC_NAND_BANK2;
  hnand1.Init.Waitfeature = FSMC_NAND_PCC_WAIT_FEATURE_ENABLE;
  hnand1.Init.MemoryDataWidth = FSMC_NAND_PCC_MEM_BUS_WIDTH_16;
  hnand1.Init.EccComputation = FSMC_NAND_ECC_DISABLE;
  hnand1.Init.ECCPageSize = FSMC_NAND_ECC_PAGE_SIZE_256BYTE;
  hnand1.Init.TCLRSetupTime = 0;
  hnand1.Init.TARSetupTime = 0;
  /* hnand1.Config */
  hnand1.Config.PageSize = 0;
  hnand1.Config.SpareAreaSize = 0;
  hnand1.Config.BlockSize = 0;
  hnand1.Config.BlockNbr = 0;
  hnand1.Config.PlaneNbr = 0;
  hnand1.Config.PlaneSize = 0;
  hnand1.Config.ExtraCommandEnable = DISABLE;
  /* ComSpaceTiming */
  ComSpaceTiming.SetupTime = 252;
  ComSpaceTiming.WaitSetupTime = 252;
  ComSpaceTiming.HoldSetupTime = 252;
  ComSpaceTiming.HiZSetupTime = 252;
  /* AttSpaceTiming */
  AttSpaceTiming.SetupTime = 252;
  AttSpaceTiming.WaitSetupTime = 252;
  AttSpaceTiming.HoldSetupTime = 252;
  AttSpaceTiming.HiZSetupTime = 252;
  HAL_NAND_Init(&hnand1, &ComSpaceTiming, &AttSpaceTiming);

  // Initialize random number generator ---------------------------------
  hrng.Instance = RNG;
  HAL_RNG_Init(&hrng);

  // Initialize SD card -----------------------------------------------------
  /* Configure GPIO pin : SDIO_Detect_Pin */
  GPIO_InitStruct.Pin = SDIO_Detect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SDIO_Detect_GPIO_Port, &GPIO_InitStruct);

  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_4B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  HAL_SD_Init(&hsd);

  // Initialize USART for debugging --------------------------------------
  husart1.Instance = USART1;
  husart1.Init.BaudRate = 9600;
  husart1.Init.WordLength = USART_WORDLENGTH_8B;
  husart1.Init.StopBits = USART_STOPBITS_1;
  husart1.Init.Parity = USART_PARITY_NONE;
  husart1.Init.Mode = USART_MODE_TX_RX;
  husart1.Init.CLKPolarity = USART_POLARITY_LOW;
  husart1.Init.CLKPhase = USART_PHASE_1EDGE;
  husart1.Init.CLKLastBit = USART_LASTBIT_DISABLE;
  HAL_USART_Init(&husart1);

  // Link fatfs drivers to sdio ------------------------------------------
  MX_FATFS_Init();
  if (retSD) while(1);

  // Initialize Timer 13 at 1MHz -----------------------------------------
  htim13.Instance = TIM13;
  htim13.Init.Prescaler = 0;
  htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim13.Init.Period = 83;
  htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&htim13);
}

/*!
 * @brief deinitialize all peripherals that were previously initialized
 * 
 * 
 */
void lowLevelDeInit(void)
{
  HAL_TIM_Base_DeInit(&htim13);

  FATFS_UnLinkDriver(SDPath);

  HAL_USART_DeInit(&husart1);

  HAL_SD_DeInit&hsd);

  HAL_RNG_DeInit(&hrng);

  HAL_NAND_DeInit(&hnand1);

  HAL_DeInit();
}