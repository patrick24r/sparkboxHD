#ifndef INC_SPARKBOXVIDEOMANAGER_H_
#define INC_SPARKBOXVIDEOMANAGER_H_

#include <vector>
#include <string>
#include "ff.h"
#include "sparkboxErrors.hpp"
#include "SparkboxGpuDataModel.h"

#ifndef MAX_FRAME_RATE
#define MAX_FRAME_RATE 60
#endif

#define MIN_FRAME_DELAY_MS (1000 / (MAX_FRAME_RATE))

using namespace std;

typedef struct SparkboxVideoDriver {
    unsigned char* gpuStartAddress;
	sparkboxError_t(*hostInitialize)(void); //< Initialize all host peripherals
	sparkboxError_t(*hostBeginDMATx)(void*, void*, unsigned int); //< Begin DMA transfer from allSamplesBuffer to 
	void (*hostDeinitialize)(void); //< Deinitialize all host peripherals
} SparkboxVideoDriver_TypeDef;

class SparkboxVideoManager
{
public:
    SparkboxVideoManager();
    ~SparkboxVideoManager();
	
    sparkboxError_t importAllSpriteLayers(string directoryPath);

    sparkboxError_t linkDriver(const SparkboxVideoDriver_TypeDef* videoDriver);
    sparkboxError_t initialize();

    void DMATransferCompleteCallback();
    void videoThreadFunction();
private:
    volatile unsigned char isInitialized = 0;
    volatile unsigned char gpuDataLocked = 0;
    volatile unsigned int totalVideoBytesImported = 0;

    const SparkboxVideoDriver_TypeDef* driver;
    vector<string> allLayerNames;
    GpuModel_TypeDef* gpuDataModel;
};

#endif /* INC_SPARKBOXVIDEOMANAGER_H_ */