#include "SparkboxVideoManager.h"
SparkboxVideoManager::SparkboxVideoManager(void)
{
	// Initialize relevant peripherals
	MX_FMC_Init();
	MX_MDMA_Init();
	// Link the mdma to the "sram" which is really the gpu
	hsram1.hmdma = &hmdma_mdma_channel40_sw_0; 

	// Allocate internal memory for video data
	gpuDataModel = new GpuModel_TypeDef;
}

int32_t SparkboxVideoManager::importAllSpriteLayers(string directoryPath)
{
	DIR dir;
	FRESULT res;
	FILINFO fno;
	int32_t addFileResult = 0;
	string filePath, fileExt;

	// Check if the folder exists
	res = f_stat(directoryPath.c_str(), &fno);
	if (res != FR_OK) return res;
	if (!(fno.fattrib & AM_DIR)) return -1;

	// Check through the folder for valid file extensions
	res = f_opendir(&dir, directoryPath.c_str());
	if (res != FR_OK) return res;

	while (res == FR_OK) {
		// On error or end of directory, break
		if (res != FR_OK || fno.fname[0] == 0) break;
		// on subdirectory, hidden file, or system file, continue
		if (fno.fattrib & (AM_DIR | AM_HID | AM_SYS)) continue;

		// Go to next file if the file is not a sprite file
		filePath.assign(fno.fname);
		if (filePath.size() < 5) continue;
		fileExt = filePath.substr(filePath.length() - 4, 4);
		for (auto& ch : fileExt) {
			ch = tolower(ch);
		}
		if (fileExt != ".spr") continue;

		// Add the sprite file
		addFileResult = importSpriteLayer(filePath);

		// Stop on any error
		if (addFileResult) return addFileResult;

		allLayerNames.push_back(filePath);
	}
	return 0;
}

int32_t SparkboxVideoManager::importSpriteLayer(string filePath)
{
	return 0;
}

void SparkboxVideoManager::resetActiveLayers() 
{
	memset(gpuDataModel->activeLayers, 0, sizeof(ActiveLayers_TypeDef) * MAX_ACTIVE_LAYERS);
}

uint8_t SparkboxVideoManager::getLayerInstanceID(uint8_t activeLayer)
{
	return gpuDataModel->activeLayers[activeLayer].layerIndex;
}
void SparkboxVideoManager::setLayerInstanceID(uint8_t activeLayer, uint8_t newLayerInstance)
{
	gpuDataModel->activeLayers[activeLayer].layerIndex = newLayerInstance;
}
int16_t SparkboxVideoManager::getXPosition(uint8_t activeLayer)
{
	return gpuDataModel->activeLayers[activeLayer].xPosition;
}
void SparkboxVideoManager::setXPosition(uint8_t activeLayer, int16_t newXPosition)
{
	gpuDataModel->activeLayers[activeLayer].xPosition = newXPosition;
}
int16_t SparkboxVideoManager::getYPosition(uint8_t activeLayer)
{
	return gpuDataModel->activeLayers[activeLayer].yPosition;
}
void SparkboxVideoManager::setYPosition(uint8_t activeLayer, int16_t newYPosition)
{
	gpuDataModel->activeLayers[activeLayer].yPosition = newYPosition;
}
int16_t SparkboxVideoManager::getXVelocity(uint8_t activeLayer)
{
	return gpuDataModel->activeLayers[activeLayer].xVelocity;
}
void SparkboxVideoManager::setXVelocity(uint8_t activeLayer, int16_t newXVelocity)
{
	gpuDataModel->activeLayers[activeLayer].xVelocity = newXVelocity;
}
int16_t SparkboxVideoManager::getYVelocity(uint8_t activeLayer)
{
	return gpuDataModel->activeLayers[activeLayer].yVelocity;
}
void SparkboxVideoManager::setYVelocity(uint8_t activeLayer, int16_t newYVelocity)
{
	gpuDataModel->activeLayers[activeLayer].xVelocity = newYVelocity;
}
uint8_t SparkboxVideoManager::getFrameIndex(uint8_t activeLayer)
{
	return gpuDataModel->activeLayers[activeLayer].frameIndex;
}
void SparkboxVideoManager::setFrameIndex(uint8_t activeLayer, uint8_t newFrameIndex)
{
	gpuDataModel->activeLayers[activeLayer].frameIndex = newFrameIndex;
}
uint8_t SparkboxVideoManager::getFontIndex(uint8_t activeLayer)
{
	return gpuDataModel->activeLayers[activeLayer].fontIndex;
}
void SparkboxVideoManager::setFontIndex(uint8_t activeLayer, uint8_t newFontIndex)
{
	gpuDataModel->activeLayers[activeLayer].fontIndex = newFontIndex;
}
uint8_t SparkboxVideoManager::getTextScale(uint8_t activeLayer)
{
	return gpuDataModel->activeLayers[activeLayer].textScale;
}
void SparkboxVideoManager::setTextScale(uint8_t activeLayer, uint8_t newTextScale)
{
	gpuDataModel->activeLayers[activeLayer].textScale = newTextScale;
}

uint16_t SparkboxVideoManager::getPaletteColor(uint8_t layer, uint8_t paletteIndex)
{
	return gpuDataModel->layersPalette[layer].colors[paletteIndex];
}