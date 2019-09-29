#pragma once

// This contains the enum for all error codes that may occur during the
// bootloading process
typedef enum {
    NO_ERROR = 0,
    NO_SD,
    BAD_SD_INIT,
    BAD_SD_READ,
    NO_GPU,
    BAD_GAME_FOLDER,
    BAD_LEVEL_FOLDER,
    BAD_LEVEL_NAME,
    BAD_LEVEL_SIZE
} bootError_t;


/* Private defines -----------------------------------------------------------*/
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOA
#define LED4_Pin GPIO_PIN_4
#define LED4_GPIO_Port GPIOA
#define LED5_Pin GPIO_PIN_5
#define LED5_GPIO_Port GPIOA
#define LED6_Pin GPIO_PIN_6
#define LED6_GPIO_Port GPIOA
#define LED7_Pin GPIO_PIN_7
#define LED7_GPIO_Port GPIOA
#define Detect_SDIO_Pin GPIO_PIN_8
#define Detect_SDIO_GPIO_Port GPIOA
