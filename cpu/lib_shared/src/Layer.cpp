#include "Layer.h"

Layer::Layer(layertype_t type, std::string initString)
{
  FRESULT error;
  uint8_t textRead[SPRITE_HEADER_BYTES];
  uint8_t numBytesRead;

  layerType = type;

  // Save user input string to class property
  layerString = initString;

  // Set palette to PALETTE_SIZE elements of all 0's
  palette.resize(PALETTE_SIZE, 0);

  // Read from disk for sprite header data
  if (layerType == SPRITE) {
    /********* SPRITE *********/

    // Import sprite header data from fatfs file system
    error = f_open(&file, textString, FA_READ);

    // If an error occured, set sprite fields to 0
    if (error) {
      spriteHeight = 0;
      spriteWidth = 0;
      numberOfFrames = 0;
      return;
    }

    // Read sprite header - includes palette, number of frames, width, height
    error = f_read(&file, textRead, SPRITE_HEADER_BYTES, &numBytesRead);
    f_close(&file);

    if (error || numBytesRead != SPRITE_HEADER_BYTES) {
      // Set sprite fields to 0 to fields to indicate error
      f_close(&file);
      spriteHeight = 0;
      spriteWidth = 0;
      numberOfFrames = 0;
      return;
    }

    // This is the only place these properties should be touched:
    // height
    // width
    // numberOfFrames
    spriteHeight = (uint16_t)(textRead[] | (textRead[] << 8));
    spriteWidth = (uint16_t)(textRead[] | (textRead[] << 8));
    numberOfFrames = (uint8_t)(textRead[]);

    // Set palette colors to data read from file
    setPalette((uint32_t*)(textRead + PALETTE_OFFSET_CHAR), PALETTE_SIZE);


    // Initilize layer flags
    // By default, sprites are animated and visible
    layerFlags = 0b1111;

  } else {
    /********* TEXT *********/
    // By default, text layers are visible
    layerFlags = 0b0101;

    // All characters visible on initialization
    numberOfVisibleCharacters = textString.size();

    // Default width, height, and font
    fontWidth = 64;
    fontHeight = 128;
    fontSelection = ARIAL;
  }
}


/*------------------ GENERIC METHODS ------------------------*/
// Get the layer header register value given a register index
uint16_t Layer::getLayerRegisterValue(uint8_t registerNumber)
{
  switch (registerNumber) {
    case 0: // Layer flags
      return layerFlags;
    case 1: // Sprite or Font width
      return getWidth();
    case 2: // Sprite or Font height
      return getHeight();
    case 3: // X position
      return (uint16_t)xPosition;
    case 4: // Y position
      return (uint16_t)yPosition;
    case 5: // Sprite - X velocity, Text - Font selection index
      if (layerType == SPRITE) return (uint16_t)xVelocity;
      else return fontSelection;
    case 6: // Sprite - Y velocity, Text - Number of visible characters
      if (layerType == SPRITE) return (uint16_t)yVelocity;
      else return numberOfVisibleCharacters;
    case 7: // Sprite - Number of frames & Current frame number, Text - Palette select
      if (layerType == SPRITE) return ((uint16_t)(currentFrameNumber) << 8) | numberOfFrames;
      else return (uint16_t)fontPaletteSelection;
    default:
      return NULL;
  }
}

// Get the layer type of the current layer
layertype_t Layer::getLayerType(void)
{
  return layerType;
}

// Set a new x position
void Layer::setxPosition(int16_t newxPosition)
{
  xPosition = newxPosition;
}

// Get the x position
uint16_t Layer::getxPosition(void)
{
  return xPosition;
}

// Set the y position
void Layer::setyPosition(int16_t newyPosition)
{
  yPosition = newyPosition;
}

// Get the y position
uint16_t Layer::getyPosition(void);
{
  return yPosition;
}

// Set the layer width
void Layer::setWidth(uint16_t newWidth)
{
  // User may only set font width,
  // sprite width is defined in the sprite file
  if (layerType == TEXT) fontWidth = newWidth;
}

// Get the layer width
uint16_t Layer::getWidth(void)
{
  if (layerType == SPRITE) return spriteWidth;
  else return fontWidth;
}

// Set the layer height
void Layer::setHeight(uint16_t newHeight)
{
  // User may only set font width,
  // sprite height is defined in the sprite file
  if (layerType == TEXT) fontHeight = newHeight;
}

// Get the layer height
uint16_t Layer::getHeight(void)
{
  if (layerType == SPRITE) return spriteHeight;
  else return fontHeight;
}

// Set current visibility status (1 = visible, 0 = invisible)
void Layer::setVisible(uint8_t visible)
{
  // Clear layer visible bit (bit 2)
  layerFlags &= ~0x0004;
  // Set bit if visible is non-zero
  layerFlags |= ((visible == 0) ? 0 : 0x0004);
}

// Get visibility status (1 = visible, 0 = invisible)
uint8_t Layer::getVisible(void)
{
  return ((layerFlags & 0x0004) != 0);
}

// Set the current Layer ID
void Layer::setLayerID(uint8_t layerID)
{
  // Reset layerID bits (15:7), then set to new layerID
  layerFlags &= ~0xFF00;
  layerFlags |= (layerID << 8);
}

// Get the current Layer ID
uint8_t Layer::getLayerID(void)
{
  return layerFlags >> 8;
}

// Clear the current palette
void Layer::clearPalette(void)
{
  // Clear all elements and set them to 0
  palette.clear();
  palette.resize(PALETTE_SIZE, 0);
}

// Set a portion of the palette
void Layer::setPalette(uint32_t *paletteData, uint8_t size)
{
  // Copy data from input buffer up to size/PALETTE_SIZE elements
  // Ignore palette slot 0, reserved for transparent
  for (uint8_t i = 1; i < size && i < PALETTE_SIZE; i++) {
    palette.at(i) = paletteData[i];
  }
}

// Set an individual palette slot value
void Layer::setPaletteAt(uint8_t position, uint32_t color)
{
  // Do not overwrite the reserved transparent slot (position 0)
  if (position != 0) palette.at(position) = color;
}

// Get an individual palette slot value
uint32_t Layer::getPaletteAt(uint8_t position)
{
  return palette.at(position);
}


/*--------------------- SPRITE METHODS ----------------------*/
// Get the number of frames
uint8_t Layer::getNumberOfFrames(void)
{
  if (layerType == SPRITE) return numberOfFrames;
  else return NULL;
}

// Set the current frame number with bounds checking
void Layer::setCurrentFrameNumber(uint8_t frame)
{
  // Make sure the current frame number is less than the 
  // total number of frames in this sprite. No lower bounds check
  // because variable is unsigned
  if (layerType == SPRITE && frame < numberOfFrames) {
    currentFrameNumber = frame;
  }
}

// Get the current frame number
uint8_t Layer::getCurrentFrameNumber(void)
{
  if (layerType == SPRITE) return currentFrameNumber;
  else return NULL;
}

// Set animated status (1 = animated, 0 = not animated)
void Layer::setAnimated(uint8_t animated)
{
  if (layerType == SPRITE){
    // Clear layer animated bit (bit 3)
    layerFlags &= ~0x0008;
    // Set bit if animated is non-zero
    layerFlags |= ((animated == 0) ? 0 : 0x0008);
  }
}

// Get animated status (1 = animated, 0 = not animated)
uint8_t Layer::getAnimated(void)
{
  if (layerType == SPRITE) return ((layerFlags & 0x0008) != 0);
  else return NULL;
}

// Get the file path to the sprite on the disk
std::string Layer::getFilePath(void)
{
  if (layerType == SPRITE) return layerString;
  else return NULL;
}

// Set the sprite x velocity
void Layer::setxVelocity(int16_t newxVelocity)
{
  if (layerType == SPRITE) xVelocity = newxVelocity;
}

// Get the sprite x velocity
int16_t Layer::getxVelocity(void)
{
  if (layerType == SPRITE) return xVelocity;
  else return NULL;
}

// Set the sprite y velocity
void Layer::setyVelocity(int16_t newyVelocity)
{
  if (layerType == SPRITE) yVelocity = newyVelocity;
}

// Get the sprite y velocity
int16_t Layer::getyVelocity(void)
{
  if (layerType == SPRITE) return yVelocity;
  else return NULL;
}

/*--------------------- TEXT METHODS ----------------------*/
// Get the text string
std::string Layer::getTextString(void)
{
  if (layerType == TEXT) return layerString;
  else return NULL;
}

// Set number of visible characters
void Layer::setNumberOfVisibleCharacters(uint16_t number);
{
  // Bounds check number of visible characters
  if (layerType == TEXT && number > layerString.size()) {
    numberOfVisibleCharacters = layerString.size();
  }
}

// Get the number of visible characters
uint16_t Layer::getNumberOfVisibleCharacters(void);
{
  if (layerType == TEXT) return numberOfVisibleCharacters;
  else return NULL;
}

// Set the font
void Layer::setFont(font_t selection)
{
  if (layerType == TEXT) fontSelection = selection;
}

// Get the font
font_t Layer::getFont(void)
{
  if (layerType == TEXT) return fontSelection;
  else return NULL;
}

// Set the font palette index
void Layer::setFontPaletteSelection(uint8_t newFontPalette)
{
  if (layerType == TEXT) fontPaletteSelection = newFontPalette;
}

// Get the font palette index
uint8_t Layer::getFontPaletteSelection(void)
{
  if (layerType == TEXT) return fontPaletteSelection;
  else return NULL;
}