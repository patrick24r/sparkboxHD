#pragma once
#include "allPeripherals.h"

#define PALETTE_SIZE 32
#define PALETTE_OFFSET_BYTES 128
#define SPRITE_HEADER_BYTES (PALETTE_OFFSET_BYTES + (PALETTE_SIZE * 4))


// This class is a container for any and all layer header information 
class Layer {
public:
    Layer(layertype_t type, std::string initString);
    ~Layer();

    /* ------------ CLASS SPECIFIC ENUMERATIONS ------------- */
    enum layertype_t : uint8_t {
        TEXT = 0,
        SPRITE = 1
    };

    enum visibility_t : uint8_t {
        INVISIBLE = 0,
        VISIBLE = 1
    };

    enum font_t : uint16_t {
        ARIAL,
        TIMESNEWROMAN,
        COURIER
    };

    /*--------------- GENERIC METHODS ------------------------*/
    /* These methods apply for both sprites and text layers */
    uint16_t getLayerRegisterValue(uint8_t registerNumber);

    layertype_t getLayerType(void);

    void setxPosition(int16_t newxPosition);
    uint16_t getxPosition(void);

    void setyPosition(int16_t newyPosition);
    uint16_t getyPosition(void);

    void setWidth(uint16_t newWidth);
    uint16_t getWidth(void);
    
    void setHeight(uint16_t newHeight);
    uint16_t getHeight(void);

    void setVisible(uint8_t visible);
    uint8_t getVisible(void);

    void setLayerID(uint8_t layerID);
    uint8_t getLayerID(void);


    // Palette specific methods
    void clearPalette(void);
    void setPalette(uint32_t *paletteData, uint8_t size);
    void setPaletteAt(uint8_t position, uint32_t color);
    uint32_t getPaletteAt(uint8_t position);


    /*--------------------- SPRITE METHODS ----------------------*/
    /* These methods apply for only sprite layers */
    uint16_t getNumberOfFrames(void);

    void setCurrentFrameNumber(uint8_t frame);
    uint8_t getCurrentFrameNumber(void);

    void setAnimated(uint8_t animated);
    uint8_t getAnimated(void);

    std::string getFilePath(void);

    void setxVelocity(int16_t newxVelocity);
    int16_t getxVelocity(void);

    void setyVelocity(int16_t newyVelocity);
    int16_t getyVelocity(void);


    /*--------------------- TEXT METHODS ----------------------*/
    /* These methods apply for only text layers */
    std::string getTextString(void);

    void setNumberOfVisibleCharacters(uint16_t number);
    uint16_t getNumberOfVisibleCharacters(void);

    void setFont(font_t selection);
    Font getFont(void);

    void setFontPaletteSelection(uint8_t paletteIndex);
    uint8_t getFontPaletteSelection(void);

private:
    /*--------------- GENERIC PROPERTIES ------------------------*/
    layertype_t layerType; // SPRITE or TEXT?
    int16_t xPosition; // Top left pixel position (0 = leftmost)
    int16_t yPosition; // Top left pixel position (0 = uppermost)
    std::string layerString; // File path for sprites, text string for text
    std::vector<uint32_t> palette; // Palette colors
    uint16_t layerFlags; // Layer flags

    /*--------------- SPRITE PROPERTIES ------------------------*/
    FIL file; // File object
    int16_t xVelocity; // Pixels / frame
    int16_t yVelocity; // Pixels / frame
    uint16_t spriteWidth; // Pixels, read from sprite header
    uint16_t spriteHeight; // Pixels, read from sprite header
    uint8_t numberOfFrames; // Number of frames on the sprite sheet
    uint8_t currentFrameNumber; // Current frame number

    /*--------------- TEXT PROPERTIES ------------------------*/
    uint16_t numberOfVisibleCharacters;
    uint16_t fontWidth;
    uint16_t fontHeight;
    font_t fontSelection;
    uint8_t fontPaletteSelection;
}