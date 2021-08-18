#ifndef INC_SPARKBOXVIDEOMANAGER_H_
#define INC_SPARKBOXVIDEOMANAGER_H_

extern "C" {
#include "stm32h7xx_hal.h"
#include "cmsis_os.h"
#include "mdma.h"
#include "fmc.h"
#include "fatfs.h"
#include "main.h"
void videoThreadWrapper(void* arg);
}

#include "SparkboxGpuDataModel.h"
#include <string>
#include <vector>

using namespace std;

class SparkboxVideoManager
{
public:
	const uint16_t MaxImportedLayers = MAX_LAYERS;
	const uint16_t MaxNumberOfFonts = MAX_FONTS;
	const uint16_t MaxActiveLayers = MAX_ACTIVE_LAYERS;
	const uint16_t MaxPaletteSlots = MAX_PALETTE_SIZE;

	SparkboxVideoManager();

	// Import Sprites
	int32_t importAllSpriteLayers(string directoryPath);
	int32_t importSpriteLayer(string filePath);
	// Add text layers
	int32_t addTextLayer(string text, uint16_t color, uint8_t& textID); // Whole string as one color
	int32_t addTextLayer(string text, vector<uint16_t> colors, vector<uint8_t>& textIDs); // Multicolor letters

	/* Global data interface functions */
	void resetAllData();

	/* Layers data interface functions */
	void resetAddedLayers();
	uint8_t getLayerID(string layerName);
	uint8_t getLayerType(uint8_t layer);
	uint16_t getHeight(uint8_t layer);
	uint16_t getWidth(uint8_t layer);

	/* Font data interface functions */
	string getFontName(uint8_t fontIndex);

	/* Active layers interface functions */
	void resetActiveLayers();
	uint8_t getLayerInstanceID(uint8_t activeLayer);
	void setLayerInstanceID(uint8_t activeLayer, uint8_t newLayerInstance);
	int16_t getXPosition(uint8_t activeLayer);
	void setXPosition(uint8_t activeLayer, int16_t newXPosition);
	int16_t getYPosition(uint8_t activeLayer);
	void setYPosition(uint8_t activeLayer, int16_t newYPosition);
	int16_t getXVelocity(uint8_t activeLayer);
	void setXVelocity(uint8_t activeLayer, int16_t newXVelocity);
	int16_t getYVelocity(uint8_t activeLayer);
	void setYVelocity(uint8_t activeLayer, int16_t newYVelocity);
	uint8_t getFrameIndex(uint8_t activeLayer);
	void setFrameIndex(uint8_t activeLayer, uint8_t newFrameIndex);
	uint8_t getFontIndex(uint8_t activeLayer);
	void setFontIndex(uint8_t activeLayer, uint8_t newFontIndex);
	uint8_t getTextScale(uint8_t activeLayer);
	void setTextScale(uint8_t activeLayer, uint8_t newTextScale);

	/* Palette interface functions */
	uint16_t getPaletteColor(uint8_t layer, uint8_t paletteIndex);

	// Callback definitions
	void callback_DMATransferComplete(void);
	// Main video thread function
	static void threadfcn_VideoManager(void* arg);
private:
	osMutexId_t frameMutexHandle;
	const osMutexAttr_t frameMutex_attributes = {
		.name = "spkFrameRenderingMutex"
	};

	osThreadId_t threadHandle;
	const osThreadAttr_t threadTask_attributes = {
		.name = "videoManagerTask",
		.stack_size = 512 * 4,
		.priority = (osPriority_t)osPriorityNormal,
	};

	vector<string> allLayerNames;
	uint32_t totalVideoBytesImported = 0;
	GpuModel_TypeDef* gpuDataModel;
};

#endif /* INC_SPARKBOXVIDEOMANAGER_H_ */