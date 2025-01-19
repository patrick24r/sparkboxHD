#pragma once

#include <variant>

#include "sparkbox/log.h"

namespace sparkbox {

enum class Status : int {
  // We good
  kOk = 0,

  // Unknown error has occurred
  kUnknown,

  // Bad input parameter
  kBadParameter,

  // Already Exists
  kAlreadyExists,

  // Timeout
  kTimeout,

  // Resource not available. Try again
  kUnavailable,

  // Bad resource state. Fix the problem and try again
  kBadResourceState,

  // Unsupported. Do not try again
  kUnsupported,
};

}  // namespace sparkbox

#define SP_RETURN_IF_ERROR(expr)               \
  do {                                         \
    const ::sparkbox::Status temp_err((expr)); \
    if (temp_err != ::sparkbox::Status::kOk) { \
      return temp_err;                         \
    }                                          \
  } while (0)

#define SP_RETURN_IF_ERROR_LOG(expr, format, ...)      \
  do {                                                 \
    const ::sparkbox::Status temp_err((expr));         \
    if (temp_err != ::sparkbox::Status::kOk) {         \
      SP_LOG_ERROR(format __VA_OPT__(, ) __VA_ARGS__); \
      return temp_err;                                 \
    }                                                  \
  } while (0)
