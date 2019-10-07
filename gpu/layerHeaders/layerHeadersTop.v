//`include "layerHeaderInfo.v"

module layerHeadersTop(
    input clk, // GPU clock
    input reset, // Reset
    input resetLayer, // Reset and clear an individual layer's data
    input [4:0] ctrlReadWriteLayer, // Controller port to r/w to given layer
    input [2:0] layerRegisterIndex, // Index of Sprite register to r/w
    input [15:0] writeLayerData, // Data to write to sprite registers
    input writeLayerEn, // Enable overwrite
    input [4:0] layer, // Layer / Sprite number
    input [10:0] pixelX, // Pixel X position
    input [10:0] pixelY, // Pixel Y position
    output reg readFlashEn, // Enable read from flash for fonts
    output reg readRamEn, // Enable read from RAM for data
    output reg [7:0] layerID, // ID number for the layer specified by the input 'layer'
    output reg [15:0] layerWidth, // Width of the font/sprite in pixels
    output reg [15:0] layerHeight, // Height of the font/sprite in pixels
    output reg signed [15:0] layerX, // X offset of layer for this pixel
    output reg signed [15:0] layerY, // Y offset of layer for this pixel
    output reg [7:0] spriteFrameIndex, // Up to 256 sprite frames per sprite sheet
    output reg [15:0] fontIndex, // Selects the font to use (Flash addr calc)
    output reg [15:0] characterIndex, // Current index of the character in text string (Flash addr calc)
    output reg [15:0] ctrlReadData // Data the controller is reading from layer headers
);
// 32 layers, each with 128 bits
// Synchronous writes (posedge), asychronous reads (constant reads)
// 8 * 16 bit "virtual registers" per sprite = 128 total bits per sprite register
// 32 total layers = 32 * 128 bit regisetrs = 4096 bits total = 512 Bytes total

// Always the current sprite layer info
wire [127:0] currLayerHeader;

// layerRegisterIndex = 3'b000 - currLayerHeader[15:0] -> Layer Flags - uint16_t
// currLayerHeader[0] -> 0 = No layer not populated, 1 = layer populated
// currLayerHeader[1] -> 0 = Text layer, 1 = Sprite layer
// currLayerHeader[2] -> 0 = layer visible, 1 = layer hidden
// currLayerHeader[3] -> 0 = not animated, 1 = animated (does not apply to text)
// currLayerHeader[4:7] -> unused
// currLayerHeader[15:8] -> Layer ID -> Up to 256 total layers, but can only use 32 at a time

// Layers are either text or traditional sprites, and the remaining body of the 
// current layer register is divided differently based on this condition


// BEGIN REGISTER INFO FOR SPRITES

// layerRegisterIndex = 3'b001 - currLayerHeader[31:16] -> Sprite Width - uint16_t
// layerRegisterIndex = 3'b010 - currLayerHeader[47:32] -> Sprite Height - uint16_t
// layerRegisterIndex = 3'b011 - currLayerHeader[63:48] -> X position (top left pixel) - int16_t
// layerRegisterIndex = 3'b100 - currLayerHeader[79:64] -> Y position (top left pixel) - int16_t
// layerRegisterIndex = 3'b101 - currLayerHeader[95:80] -> X Velocity (Pixels / second) - int16_t
// layerRegisterIndex = 3'b110 - currLayerHeader[111:96] -> Y Velocity (Pixels / second) - int16_t
// layerRegisterIndex = 3'b111 - currLayerHeader[119:112] -> Number of frames in sprite sheet - uint8_t
// layerRegisterIndex = 3'b111 - currLayerHeader[127:120] -> Current frame number - uint8_t

// END REGISTER INFO FOR SPRITES



// BEGIN REGISTER INFORMATION FOR TEXT

// layerRegisterIndex = 3'b001 - currLayerHeader[31:16] -> Font width (pixels) - uint16_t
// layerRegisterIndex = 3'b010 - currLayerHeader[47:32] -> Font height (pixels) - uint16_t
// layerRegisterIndex = 3'b011 - currLayerHeader[63:48] -> X position (top left pixel) - int16_t
// layerRegisterIndex = 3'b100 - currLayerHeader[79:64] -> Y position (top left pixel) - int16_t
// layerRegisterIndex = 3'b101 - currLayerHeader[95:80] -> Font selection index (see font controller for details) - uint16_t
// layerRegisterIndex = 3'b110 - currLayerHeader[111:96] -> Number of characters in Text Layer - uint16_t
// layerRegisterIndex = 3'b111 - currLayerHeader[127:112] -> Font palette index - uint16_t

// END REGISTER INFORMATION FOR TEXT

// Wire for pixel offset calculations

// Wire for read memory enable bits
// Memory module for layer header info
layerHeaderInfo layer_head_inst(
    clk, // Master clock
    reset, // 0 = reset, 1 = no reset
    resetLayer, // 0 = reset, 1 = no reset
    layer, // Layer header to read for pipeline
    ctrlReadWriteLayer, // Layer header to read/write for controller
    layerRegisterIndex, // Index of the layer register to write/read (only used by controller and defined in layerInfoTop.sv)
    writeLayerEn, // 1 = write enabled, 0 = write disabled
    writeLayerData, // Data to write to specified register
    currLayerHeader, // All layer header info that was read (for pipeline)
    ctrlReadData // Data controller is reading (for controller)
);

// -----------------------------------------------------------------------------------
// NOTE: The controller is the only actor writing data to places. This means for 
// X and Y velocity updating X and y position and animations updating frame numbers
// the controller must be the actor to write this data back to the header registers.
// This action is performed at the beginning of a new frame before any processing
// -----------------------------------------------------------------------------------

// Assign outputs based on header information read
always begin
    // Determine if reading RAM and Flash is necessary
    if (currLayerHeader[0]) begin
        // Layer populated
        if (currLayerHeader[1]) begin
            // Sprite layer

            // Set text fields to 0
            readFlashEn <= 1'b0;

            // Read RAM only if sprite is on current pixel 
            readRamEn <= 
                (layerX + 1) > 0 && // Check X bounds (sprite width)
                layerX < currLayerHeader[31:16] &&
                (layerY + 1) > 0 && // Check Y bounds (sprite height)
                layerY < currLayerHeader[47:32];
        end else begin
            // Text layer

            // Read flash only if reading RAM
            readFlashEn <= 
					 (layerX + 1) > 0 && // Check X bounds (num chars * font width)
                layerX < currLayerHeader[31:16] * currLayerHeader[111:96] &&
                (layerY + 1) > 0 && // Check Y bounds (font height)
                layerY < currLayerHeader[47:32];

            // Read RAM to find character if text covers pixel
            readRamEn <= 
                (layerX + 1) > 0 && // Check X bounds (num chars * font width)
                layerX < currLayerHeader[31:16] * currLayerHeader[111:96] &&
                (layerY + 1) > 0 && // Check Y bounds (font height)
                layerY < currLayerHeader[47:32];
        end
    end else begin
        // Layer not populated, not reading RAMs or Flash
        readRamEn <= 1'b0;
        readFlashEn <= 1'b0;
    end

    // Wire out the layer ID
    layerID <= currLayerHeader[15:8];

    // Find layer height and width
    layerWidth <= currLayerHeader[31:16];
    layerHeight <= currLayerHeader[47:32];

    // Find X and Y offset for this layer on this pixel
    layerX <= pixelX - currLayerHeader[63:48];
    layerY <= pixelY - currLayerHeader[79:64];

    // Wire out sprite frame index
    spriteFrameIndex <= currLayerHeader[127:120];

    // Wire out font index
    fontIndex <= currLayerHeader[95:80];

    // Find index of character to draw on current pixel
    characterIndex <= $unsigned(layerX) / $unsigned(currLayerHeader[95:80]);

    
end

endmodule