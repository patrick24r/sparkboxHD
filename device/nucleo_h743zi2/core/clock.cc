#include "nucleo_h743zi2/core/clock.h"

#include "FreeRTOS.h"
#include "nucleo_h743zi2/core/interrupt.h"
#include "sparkbox/assert.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"

namespace {

static TIM_HandleTypeDef htim7;

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
  RCC_ClkInitTypeDef clkconfig;
  uint32_t uwTimclock, uwAPB1Prescaler;
  uint32_t uwPrescalerValue, pFLatency;

  if (TickPriority < (1UL << __NVIC_PRIO_BITS)) {
    HAL_NVIC_SetPriority(TIM7_IRQn, TickPriority, 0U);

    /* Enable the TIM7 global Interrupt */
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
    uwTickPrio = TickPriority;
  } else {
    return HAL_ERROR;
  }

  /* Enable TIM7 clock */
  __HAL_RCC_TIM7_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  // Compute TIM7 clock
  if (clkconfig.APB1CLKDivider == RCC_HCLK_DIV1) {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  } else {
    uwTimclock = 2UL * HAL_RCC_GetPCLK1Freq();
  }

  // Compute the prescaler value to have TIM7 counter clock equal to 1MHz
  uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000U) - 1U);

  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM7CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  htim7.Instance = TIM7;
  htim7.Init.Period = (1000000U / 1000U) - 1U;
  htim7.Init.Prescaler = uwPrescalerValue;
  htim7.Init.ClockDivision = 0;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;

  SP_ASSERT(HAL_TIM_Base_Init(&htim7) == HAL_OK);
  return HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_SuspendTick(void)
{
  /* Disable TIM7 update Interrupt */
  __HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);
}

void HAL_ResumeTick(void)
{
  /* Enable TIM7 Update interrupt */
  __HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);
}

} // namespace


namespace nucleoh743zi2::core {

InterruptManager::Callback tim7_cb = TIM7_IT_Handler;
void TIM7_IT_Handler(void) {
  HAL_TIM_IRQHandler(&htim7);
}

InterruptManager::Callback systick_cb = SysTick_IT_Handler;
void SysTick_IT_Handler(void) {
  // Clear overflow flag
  SysTick->CTRL;

  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    // Call tick handler
    xPortSysTickHandler();
  }
}

void Clock::SetUp() {
  // Reset of all peripherals, Initializes the Flash interface and the Systick
  HAL_Init();

  // Initialize core clock to 480 MHz assuming 8 MHz HSE
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY));

  // Initialize PLL
  RCC_OscInitTypeDef RCC_OscInitStruct = {
    .OscillatorType = RCC_OSCILLATORTYPE_HSE,
    .HSEState = RCC_HSE_BYPASS,
    .PLL = {
      .PLLState = RCC_PLL_ON,
      .PLLSource = RCC_PLLSOURCE_HSE,
      .PLLM = 2,
      .PLLN = 240,
      .PLLP = 2,
      .PLLQ = 2,
      .PLLR = 2,
      .PLLRGE = RCC_PLL1VCIRANGE_2,
      .PLLVCOSEL = RCC_PLL1VCOWIDE,
      .PLLFRACN = 0
    },
  };
  SP_ASSERT(HAL_RCC_OscConfig(&RCC_OscInitStruct) == HAL_OK);

  // Initializes the CPU, AHB and APB buses clocks
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {
    .ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | \
                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | \
                 RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1,
    .SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
    .SYSCLKDivider = RCC_SYSCLK_DIV1,
    .AHBCLKDivider = RCC_HCLK_DIV2,
    .APB3CLKDivider = RCC_APB3_DIV2,
    .APB1CLKDivider = RCC_APB1_DIV2,
    .APB2CLKDivider = RCC_APB2_DIV2,
    .APB4CLKDivider = RCC_APB4_DIV2
  };
  SP_ASSERT(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) == HAL_OK);

  // Initialize TIM7 as systick base at 1 kHz
  
  // Register interrupt callbacks
  InterruptManager::Instance().RegisterCallback(TIM7_IRQn, tim7_cb);
  InterruptManager::Instance().RegisterCallback(SysTick_IRQn, systick_cb);
}

void Clock::TearDown() {

}

} // namepsace nucleoh743zi2::core