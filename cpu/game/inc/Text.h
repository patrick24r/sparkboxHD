#pragma once
#include "Layer.h"
#include <string>

class Text : Layer {
public:
    enum Font : uint16_t {
        arial,
        timesNewRoman
    };

    // Changing the string contents requires writing to GPU RAM, (not simple)
    // Therefore, 
    std::String textString;

    Font fontSelection;

private:
    
    // Number of characters should auto update as textString changes
    uint16_t numberOfCharacters;


    
}