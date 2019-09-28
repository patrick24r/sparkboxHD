#pragma once
#include "Layer.h"

// This class is used to maintain information
// Main problem is keeping sprite data on GPU and on CPU synchronized
class Sprite : Layer {
public:
    Sprite();
    ~Sprite();

    

private:
    // Variables are 
    int16_t xVelocity; // Pixels / frame
    int16_t yVelocity; // Pixels / frame
    uint16_t numberOfFrames; // Number of frames on the sprite sheet
    uint16_t currentFrameNumber; // Current frame number
}