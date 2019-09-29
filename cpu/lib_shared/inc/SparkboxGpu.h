// This class contains low level functions to interface
// with the Sparkbox GPU
#include "PeripheralHandles.h"

class SparkboxGpu
{
public:
  // Constructor and Destructor
  SparkboxGpu();
  // Allow this function to be called without instantiating
  static uint16_t generateGpuCommand(GPU_COMMAND_TYPE type, GPU_COMMAND_TARGET target, uint16_t parameters);

  static uint16_t generateGpuCommand((GPU_COMMAND_TYPE type, GPU_COMMAND_TARGET target);

  static uint16_t sendGpuCommand(uint16_t command, uint16_t data);

private:

}


/* **************************************************************************
 *                              COMMAND USAGE
 * **************************************************************************
 * Commands are 16 bits [15:0] and are accompanied by 16 bit data
 * command[15:14] - Command Type
 *      00 - Reset
 *      01 - Read
 *      10 - Write
 *      11 - Reserved
 *          (potentially used to dynamically change size in
 *          RAM/Flash of each sprite/font)
 *
 *
 * command[13:11] - Target GPU memory
 *      000 - All GPU memories
 *      001 - layer headers
 *      010 - RAM (layer data storage)
 *      011 - Palette
 *      100 - Flash (text glyph data)
 *     101 - Reserved
 *      11X - Reserved
 *
 *
 * COMMAND PARAMETERS
 * If targeting pallete, layer headers, or RAM memory
 * --------------------------------------------------
 * command[5:0]
 *      000000 - Layer 0
 *      000001 - Layer 1
 *      ...
 *      ...
 *      011111 - Layer 31
 *      1xxxxx - All layers
 *
 * If targeting layer headers
 * ------------------------------------------------
 * command[8:6] - Layer header register address
 * command[10:9] - Reserved
 *
 * If targeting palette
 * ------------------------------------------------
 * command[10:6] - Palette index
 *
 * If targeting RAM
 * ------------------------------------------------
 * command[10:6] - Reserved
 *
 *
 * If targeting Flash memory (Should not be done by normal game)
 * ---------------------------------------------------
 * command[10:0] - Font index (2^11 theoretical possible fonts)
 * Only target flash if you really know what you're doing
 *
 * Each font takes up a fixed number of bytes in flash memory
 * Therefore only an index number is needed to determine the start write address
 *
 *
 * Notes: NOT all combinations of command pieces are supported
 * For example, reading/writing all gpu memories is not supported
 * Block reads/writes: RAM and Flash memories use block reads/writes
 * For block writes, continuously clock in the same write command (used to find start address)
 * and update data line bit values
 * For block reads, continuously clock in the same read command (used to find start address)
 * and data lines will update with read data
 *
 */

/*
 * The following enums provide users a simple way of constructing GPU commands
 * Usage examples:
 *
 * // Write to palette spot 0 for layer 0
 * cmd = generateGpuCommand(GPU_WRITE, GPU_PALETTE, GPU_LAYER_0 | GPU_PALETTE_0);
 * sendGpuCommand(cmd, data);
 *
 * // Read layer 3's font number (assuming it is a text layer)
 * cmd = generateGpuCommand(GPU_READ, GPU_LAYERHEADERS, GPU_LAYER_3 | GPU_HEADER_FONT);
 * readBack = sendGpuCommand(cmd, data);
 *
 * // Write a sprite sheet's data to layer 4's RAM storage
 * cmd = generateGpuCommand(GPU_WRITE, GPU_RAM, GPU_LAYER_4);
 * for (i = 0; i < sprite.width * sprite.height * sprite.numberOfFrames; i++) {
 *      sendGpuCommand(cmd, spriteData[i]);
 * }
 *
 */
enum GPU_COMMAND_TYPE : uint8_t {
    GPU_RESET = 0,  // 0b00
    GPU_READ = 1,   // 0b01
    GPU_WRITE = 2   // 0b10
};

enum GPU_COMMAND_TARGET : uint8_t {
    GPU_ALL = 0,
    GPU_LAYERHEADERS = 1,
    GPU_RAM = 2,
    GPU_PALETTE = 3,
    GPU_FLASH = 4
};


enum GPU_LAYER_PARAMETER : uint16_t {
    GPU_LAYER_0 = 0, GPU_LAYER_1 = 1, GPU_LAYER_2 = 2, GPU_LAYER_3 = 3,
    GPU_LAYER_4 = 4, GPU_LAYER_5 = 5, GPU_LAYER_6 = 6, GPU_LAYER_7 = 7,
    GPU_LAYER_8 = 8, GPU_LAYER_9 = 9, GPU_LAYER_10 = 10, GPU_LAYER_11 = 11,
    GPU_LAYER_12 = 12, GPU_LAYER_13 = 13, GPU_LAYER_14 = 14, GPU_LAYER_15 = 15,
    GPU_LAYER_16 = 16, GPU_LAYER_17 = 17, GPU_LAYER_18 = 18, GPU_LAYER_19 = 19,
    GPU_LAYER_20 = 20, GPU_LAYER_21 = 21, GPU_LAYER_22 = 22, GPU_LAYER_23 = 23,
    GPU_LAYER_24 = 24, GPU_LAYER_25 = 25, GPU_LAYER_26 = 26, GPU_LAYER_27 = 27,
    GPU_LAYER_28 = 28, GPU_LAYER_29 = 29, GPU_LAYER_30 = 30, GPU_LAYER_31 = 31,
    GPU_LAYER_ALL = 32
};

// Used only when reading/writing layer header registers
enum GPU_LAYER_HEADER : uint16_t {
    GPU_HEADER_FLAGS = 0 << 6,
    GPU_HEADER_WIDTH = 1 << 6,
    GPU_HEADER_HEIGHT = 2 << 6,
    GPU_HEADER_XPOS = 3 << 6,
    GPU_HEADER_YPOS = 4 << 6,
    GPU_HEADER_XVELOCITY = 5 << 6,
    GPU_HEADER_YVELOCITY = 6 << 6,
    GPU_HEADER_NUMBERFRAMES = 7 << 6,
    GPU_HEADER_CURRENTFRAME = 7 << 6,
    GPU_HEADER_FONT = 5 << 6,
    GPU_HEADER_NUMBERCHARACTERS = 6 << 6,
    GPU_HEADER_FONTPALETTE = 7 << 6
};

// Only used when reading/writing palettes
enum GPU_PALETTE : uint16_t {
    GPU_PALETTE_0 = 0 << 6,
    GPU_PALETTE_1 = 1 << 6,
    GPU_PALETTE_2 = 2 << 6,
    GPU_PALETTE_3 = 3 << 6,
    GPU_PALETTE_4 = 4 << 6,
    GPU_PALETTE_5 = 5 << 6,
    GPU_PALETTE_6 = 6 << 6,
    GPU_PALETTE_7 = 7 << 6,
    GPU_PALETTE_8 = 8 << 6,
    GPU_PALETTE_9 = 9 << 6,
    GPU_PALETTE_10 = 10 << 6,
    GPU_PALETTE_11 = 11 << 6,
    GPU_PALETTE_12 = 12 << 6,
    GPU_PALETTE_13 = 13 << 6,
    GPU_PALETTE_14 = 14 << 6,
    GPU_PALETTE_15 = 15 << 6,
    GPU_PALETTE_16 = 16 << 6,
    GPU_PALETTE_17 = 17 << 6,
    GPU_PALETTE_18 = 18 << 6,
    GPU_PALETTE_19 = 19 << 6,
    GPU_PALETTE_20 = 20 << 6,
    GPU_PALETTE_21 = 21 << 6,
    GPU_PALETTE_22 = 22 << 6,
    GPU_PALETTE_23 = 23 << 6,
    GPU_PALETTE_24 = 24 << 6,
    GPU_PALETTE_25 = 25 << 6,
    GPU_PALETTE_26 = 26 << 6,
    GPU_PALETTE_27 = 27 << 6,
    GPU_PALETTE_28 = 28 << 6,
    GPU_PALETTE_29 = 29 << 6,
    GPU_PALETTE_30 = 30 << 6,
    GPU_PALETTE_31 = 31 << 6
};
