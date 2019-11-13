#pragma once
#include "Layer.h"
#include <string>

class Text : Layer {
public:
    enum Font : uint16_t {
        arial,
        timesNewRoman,
        courier
    };

    Text(std::string text);

    
    uint16_t fontWidth;
    uint16_t fontHeight;
    Font fontSelection;

private:
    // Changing the string contents requires writing to GPU RAM, (not simple)
    // Therefore, 
    std::string textString;
    // Number of characters should auto update as textString changes
    uint16_t numberOfCharacters;

    
}