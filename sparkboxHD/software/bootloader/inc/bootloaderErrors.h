#pragma once

// This cntains the enum for all error codes that may occur during the
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