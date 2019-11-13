#pragma once
#include "ff.h"
#include "SparkboxGpu.h"
#include <vector>

virtual class Layer {
public:
    Layer(void){
        // Set palette to 32 elements of all 0's
        palette.resize(32, 0);
    }

    void clearPalette(void){
        // Clear all elements and set them to 0
        palette.clear();
        palette.resize(32, 0);
    }

    void setPalette(uint32_t *paletteData, uint8_t size)
    {
        // If the vector passed in has the same size
        // as the palette, copy newPalette into palette
        for (uint8_t i = 0; i < size; i++) {
            palette.at(i) = i == 0 ? 0 : paletteData[i];
        }
    }

    void setPaletteAt(uint8_t position, uint32_t color){
        // Do not overwrite the reserved transparent slot (position 0)
        if (position != 0) palette.at(position) = color;
    }

    uint32_t getPaletteAt(uint8_t position){
        return palette.at(position);
    }

    // Public properties
    int16_t xPosition; // Top left pixel position (0 = leftmost)
    int16_t yPosition; // Top left pixel position (0 = uppermost)
private:
    std::vector<uint32_t> palette; // Palette colors
    uint16_t layerFlags; // Layer flags
}