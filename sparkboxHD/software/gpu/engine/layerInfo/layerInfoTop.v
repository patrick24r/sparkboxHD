`include "layerHeaderInfo.v"

module layerInfoTop(
    input clk, // Master clock
    input reset, // Reset and clear sprite data
    input [4:0] writeLayerIndex, // Write to given layer
    input [2:0] layerRegisterIndex, // Index of Sprite register to write
    input [15:0] writeLayerData, // Data to write to sprite registers
    input writeLayerEn, // Enable overwrite
    input [4:0] layer, // Layer / Sprite number
    input [10:0] pixelX, // Pixel X position
    input [10:0] pixelY, // Pixel Y position
    output readFlashEn, // Enable read from flash for fonts
    output readRamEn, // Enable read from RAM for data
    output [15:0] fontIndex, // Selects the font to use (Flash addr calc)
    output [15:0] characterIndex, // Current index of the character in text string (Flash addr calc)
    output [15:0] fontWidth, // Width of the font in pixels (Pixel value calc)
    output [15:0] layerX, // X offset of layer for this pixel
    output [15:0] layerY, // Y offset of layer for this pixel
    output [7:0] spriteFrameIndex // Up to 2048 sprite frames per sprite sheet
);

// 32 layers, each with 128 bits
// Synchronous writes (posedge), asychronous reads (constant reads)
// 8 * 16 bit "virtual registers" per sprite = 128 total bits per sprite register
// 32 total layers = 32 * 128 bit regisetrs = 4096 bits total = 512 Bytes total

// Always the current sprite layer info
wire [127:0] currLayerHeader;

// layerRegisterIndex = 4'b0000 - currLayerHeader[15:0] -> Layer Flags - uint16_t
// currLayerHeader[0] -> 0 = No layer not populated, 1 = layer populated
// currLayerHeader[1] -> 0 = Text layer, 1 = Sprite layer
// currLayerHeader[2] -> 0 = layer visible, 1 = layer hidden
// currLayerHeader[3] -> 0 = not animated, 1 = animated (does not apply to text)
// currLayerHeader[15:4] -> Unused

// Layers are either text or traditional sprites, and the remaining body of the 
// current layer register is divided differently based on this condition


// BEGIN REGISTER INFO FOR SPRITES

// layerRegisterIndex = 3'b001 - currLayerHeader[31:16] -> Sprite Width - uint16_t
// layerRegisterIndex = 3'b010 - currLayerHeader[47:32] -> Sprite Height - uint16_t
// layerRegisterIndex = 3'b011 - currLayerHeader[63:48] -> X position (Top left pixel of sprite) - int16_t
// layerRegisterIndex = 3'b100 - currLayerHeader[79:64] -> Y position (Top left pixel of sprite) - int16_t
// layerRegisterIndex = 3'b101 - currLayerHeader[95:80] -> X Velocity (Pixels / second) - int16_t
// layerRegisterIndex = 3'b110 - currLayerHeader[111:96] -> Y Velocity (Pixels / second) - int16_t
// layerRegisterIndex = 3'b111 - currLayerHeader[119:112] -> Number of frames in sprite sheet - uint8_t
// layerRegisterIndex = 3'b111 - currLayerHeader[127:120] -> Current frame number - uint8_t

// END REGISTER INFO FOR SPRITES



// BEGIN REGISTER INFORMATION FOR TEXT

// layerRegisterIndex = 3'b001 - currLayerHeader[31:16] -> Font width (pixels) - uint16_t
// layerRegisterIndex = 3'b010 - currLayerHeader[47:32] -> Font height (pixels) - uint16_t
// layerRegisterIndex = 3'b011 - currLayerHeader[63:48] -> Text X position (top left pixel) - int16_t
// layerRegisterIndex = 3'b100 - currLayerHeader[79:64] -> Text Y position (top left pixel) - int16_t
// layerRegisterIndex = 3'b101 - currLayerHeader[95:80] -> Font selection index (see font controller for details) - uint16_t
// layerRegisterIndex = 3'b110 - currLayerHeader[111:96] -> Number of characters in Text Layer - uint16_t
// layerRegisterIndex = 3'b111 - currLayerHeader[127:112] -> Font palette index - uint16_t

// END REGISTER INFORMATION FOR TEXT

// Wire for pixel offset calculations

// Wire for read memory enable bits
wire layerOnPixel;

// Memory module for layer header info
layerHeaderInfo inst_0(
    clk, // Master clk
    reset, // 0 = reset, will slear all header data
    layer, // layer to read header data from
    writeLayerIndex, // Layer to write header data to 
    layerRegisterIndex, // Register of header data to write to
    writeLayerEn, // Enable / disable write to header memory
    writeLayerData, // Data to write to header memory
    currLayerHeader // Output from reading header info
);


// Assign outputs based on header information read
always @(*) begin
    if (currLayerHeader[0])
        // Layer populated

        // Find X and Y offset for this layer on this pixel
        layerX <= pixelX - currLayerHeader[63:48];
        layerY <= pixelY - currLayerHeader[79:64];

        // Wire out font index
        fontIndex <= currLayerHeader[95:80];

        // Wire out sprite frame index
        spriteFrameIndex <= currLayerHeader[127:120];

        // Find index of character to draw on current pixel
        characterIndex <= layerX / currLayerHeader[95:80];

        // Read RAM if the current layer is on the current pixel
        readRamEn <= layerOnPixel;

        if (currLayerHeader[1]) begin
            // Sprite layer

            // Set text fields to 0
            readFlashEn <= 1'b0;

            // Read RAM only if sprite is on current pixel 
            layerOnPixel <= 
                ((signed)layerX + 1) > 0 && // Check X bounds
                layerX < currLayerHeader[31:16] &&
                ((signed)layerY + 1) > 0 && // Check Y bounds
                layerY < currLayerHeader[47:32];
        end
        else begin
            // Text layer

            // Read flash only if reading RAM
            readFlashEn <= layerOnPixel;

            // Read RAM to find character if text covers pixel
            layerOnPixel <= 
                ((signed)layerX + 1) > 0 && // Check X bounds
                layerX < currLayerHeader[95:80] * currLayerHeader[47:32] &&
                ((signed)layerY + 1) > 0 && // Check Y bounds
                layerY < currLayerHeader[111:96] + currLayerHeader[79:64];
        end
    end
    else begin
        // Layer not populated
        // internal wire is 0
        layerOnPixel = 1'b0;
        // All outputs are 0
        readFlashEn = 1'b0;
        readRamEn = 1'b0;
        fontIndex = 16'd0;
        characterIndex = 16'd0;
        layerX = 16'd0;
        layerY = 16'd0;
        spriteFrameIndex = 8'd0;

    end
end

endmodule