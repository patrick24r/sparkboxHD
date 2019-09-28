#pragma once
#include "SparkboxGpu.h"
#include <vector>

class Layer {
public:

    getWidth();
    virtual setWidth(uint16_t newWidth, SparkboxGpu spkGpu){
        width = newWidth;
        spkGpu.
    }
    getHeight();
    virtual setHeight(uint16_t newHeight);

    

    // Synchronizes layer header data between GPU and CPU
    virtual synchronizeLayer();

    // Palette colors
    std::vector<uint32_t> palette;

private:
    uint16_t layerFlags; //
    uint16_t width; // Pixels
    uint16_t height; // Pixels
    int16_t xPosition; // Top left pixel position (0 = leftmost)
    int16_t yPosition; // Top left pixel position (0 = uppermost)

    // This property is private to bounds check when setting the current layer
    uint8_t currentLayer;
}