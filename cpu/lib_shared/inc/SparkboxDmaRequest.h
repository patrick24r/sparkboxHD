#pragma once
#include <cstdint>

class SparkboxDmaRequest {
  SparkboxDmaRequest();
  
  uint8_t *transferFromAddress;
  uint8_t *transferToAddress;
  uint32_t numberOfBytes;
};