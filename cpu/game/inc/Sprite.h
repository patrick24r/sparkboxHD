#pragma once
#include "Layer.h"

#define SPRITE_HEADER_BYTES 200

// This class is used to maintain information
// Main problem is keeping sprite data on GPU and on CPU synchronized
class Sprite : Layer {
public:
    Sprite(String fullfile);
    ~Sprite();

    // Setters and getters for private variables
    uint16_t getNumberOfFrames(void);

    // Setters and getters for private layer flags
    void setAnimated(uint8_t animated);
    uint8_t getAnimated(void);
    void setVisible(uint8_t visible);
    uint8_t getVisible(void);
    void setLayerID(uint8_t layerID);
    uint8_t getLayerID(void);
    uint16_t getWidth(void);
    uint16_t getHeight(void);


    // Public Sprite specific properties
    int16_t xVelocity; // Pixels / frame
    int16_t yVelocity; // Pixels / frame
private:
    // Provate properties
    String filePath; // Path to file object on disk
    FIL file; // File object
    uint16_t width; // Pixels, read from sprite header
    uint16_t height; // Pixels, read from sprite header
    uint8_t numberOfFrames; // Number of frames on the sprite sheet
    uint8_t currentFrameNumber; // Current frame number
}