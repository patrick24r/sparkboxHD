#pragma once
#include <cstdint>

class SparkboxDmaRequest {
  SparkboxDmaRequest();
  
  uint32_t transferFromAddress;
  uint32_t transferToAddress;
  uint32_t numberOfBytes;
};