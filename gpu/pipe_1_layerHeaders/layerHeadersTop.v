//`include "layerHeaderInfo.v"

module layerHeadersTop(
    input clk, // Pipeline clock
    input reset, // Reset
    input resetLayer, // Reset and clear an individual layer's data
    input [4:0] ctrlReadWriteLayer, // Controller port to r/w to given layer
    input [2:0] layerRegisterIndex, // Index of Sprite register to r/w for controller
    input [15:0] writeLayerData, // Data to write to sprite registers
    input writeLayerEn, // Enable overwrite
    input [4:0] layer, // Layer / Sprite number
    output [15:0] ctrlReadData, // Data read by the controller
    output [127:0] currLayerHeader // Always the current sprite layer info
);
// 32 layers, each with 128 bits
// Synchronous writes (posedge), asychronous reads (constant reads)
// 8 * 16 bit "virtual registers" per sprite = 128 total bits per sprite register
// 32 total layers = 32 * 128 bit regisetrs = 4096 bits total = 512 Bytes total

// -----------------------------------------------------------------------------------
// NOTE: The controller is the only actor writing data to places. This means for 
// X and Y velocity updating X and y position and animations updating frame numbers
// the controller must be the actor to write this data back to the header registers.
// This action is performed at the beginning of a new frame before any processing
// -----------------------------------------------------------------------------------

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
    !clk, // pipeline clock inverted - write on negative edges to complete in 1 cycle
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



endmodule