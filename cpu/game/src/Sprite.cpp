#include "Sprite.h"

// Constructor, input 'fullfile' is full
// path on file system to the sprite file
Sprite::Sprite(String fullfile)
{
  FRESULT error;
  uint8_t textRead[SPRITE_HEADER_BYTES];
  uint8_t numBytesRead;

  // Import sprite header data from file system
  filePath = fullfile;
  
  error = f_open(&file, filePath, FA_READ);

  if (error) {
    // Do something to handle this file open error
  }

  // Read sprite header - includes palette, number of frames, width, height
  error = f_read(&file, textRead, SPRITE_HEADER_BYTES, &numBytesRead);
  f_close(&file);
  if (error || numBytesRead != SPRITE_HEADER_BYTES) {
    // Do something to handle this file read error
  }

  // This is the only place these properties should be touched:
  // height
  // width
  // numberOfFrames
  height = (uint16_t)(textRead[] | (textRead[] << 8));
  width = (uint16_t)(textRead[] | (textRead[] << 8));
  numberOfFrames = (uint8_t)(textRead[]);

  // Set palette colors to data read from file
  setPalette((uint32_t*)(textRead + PALETTE_OFFSET_CHAR), 32);


  // Initilize layer flags
  // By default, sprites are animated and visible
  layerFlags = 0b1011;
}



// Setters and getters for sprite flags
void Sprite::setAnimated(uint8_t animated)
{
  // Clear layer animated bit (bit 3)
  layerFlags &= ~0x0008;
  // Set bit if animated is non-zero
  layerFlags |= ((animated == 0) ? 0 : 0x0008);
}
uint8_t Sprite::getAnimated(void)
{
  return ((layerFlags & 0x0008) != 0)
}

void Sprite::setVisible(uint8_t visible)
{
  // Clear layer visible bit (bit 2)
  layerFlags &= ~0x0004;
  // Set bit if visible is non-zero
  layerFlags |= ((visible == 0) ? 0 : 0x0004);
}
uint8_t Sprite::getVisible(void)
{
  return ((layerFlags & 0x0004) != 0);
}


void Sprite::setLayerID(uint8_t layerID)
{
  // Reset layerID bits (15:7), then set to new layerID
  layerFlags &= ~0xFF00;
  layerFlags |= (layerID << 8);
}
uint8_t Sprite::getLayerID(void);
{
  return layerFlags >> 8;
}

uint16_t getWidth(void)
{
  return width;
}
uint16_t getHeight(void)
{
  return height;
}