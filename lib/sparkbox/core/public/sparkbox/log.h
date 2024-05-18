#pragma once

#include <cstdio>

// Device calls std::freopen(stderr)

#define LOG_STRING_ERROR "ERROR"
#define LOG_STRING_WARN "WARN"
#define LOG_STRING_INFO "INFO"
#define LOG_STRING_DEBUG "DEBUG"

// If not overwritten,
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#ifndef LOG_OUTPUT
#define LOG_OUTPUT stdout
#endif

// Logging macros, all output to stderr
#define SP_LOG_BASE(level, message, ...)                                     \
  do {                                                                       \
    fprintf(LOG_OUTPUT, "%-5s | %-25s:%d | %s: ", level, __FILE__, __LINE__, \
            __FUNCTION__);                                                   \
    fprintf(LOG_OUTPUT, message __VA_OPT__(, ) __VA_ARGS__);                 \
    fprintf(LOG_OUTPUT, "\n");                                               \
  } while (0)

#define SP_LOG_ERROR(format, ...) \
  SP_LOG_BASE(LOG_STRING_ERROR, format __VA_OPT__(, ) __VA_ARGS__)

#define SP_LOG_WARN(format, ...) \
  SP_LOG_BASE(LOG_STRING_WARN, format __VA_OPT__(, ) __VA_ARGS__)

#define SP_LOG_INFO(format, ...) \
  SP_LOG_BASE(LOG_STRING_INFO, format __VA_OPT__(, ) __VA_ARGS__)

#define SP_LOG_DEBUG(format, ...) \
  SP_LOG_BASE(LOG_STRING_DEBUG, format __VA_OPT__(, ) __VA_ARGS__)