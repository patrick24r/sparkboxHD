// This module calculates address offsets for RAM and flash reads
module aluTop(
    input [127:0] currLayerHeader, // Current layer header data
	 input [10:0] pixelX, // Current X pixel position
	 input [10:0] pixelY, // Current Y pixel position
    output reg readFlashEn, // Enable read from flash for fonts
    output reg readRamEn, // Enable read from RAM for data
    output reg [7:0] layerID, // ID number for the layer specified by the input 'layer'
    output [25:0] ramAddressOffset, // Address offset for RAM (words)
    output [25:0] flashAddressBits, // Address for flash (bits)
	 output reg unsigned [15:0] layerX,
	 output reg unsigned [15:0] layerY,
	 output reg unsigned [15:0] characterIndex
);

reg unsigned [15:0] layerWidth; // Width of the font/sprite in pixels
reg unsigned [15:0] layerHeight; // Height of the font/sprite in pixels
//reg unsigned [15:0] layerX; // X offset of layer for this pixel
//reg unsigned [15:0] layerY; // Y offset of layer for this pixel
reg unsigned [7:0] spriteFrameIndex; // Up to 256 sprite frames per sprite sheet
reg unsigned [15:0] fontIndex; // Selects the font to use (Flash addr calc)
// reg unsigned [15:0] characterIndex; // Current index of the character in text string (Flash addr calc)

ramAddressCalc inst_ramAddressCalc(
    .isSprite(currLayerHeader[1]),
    .frameNumber(spriteFrameIndex),
    .height(layerHeight),
    .width(layerWidth),
    .xOffset(layerX),
    .yOffset(layerY),
    .characterIndex(characterIndex),
    .addressOffset(ramAddressOffset)
);

flashAddressCalc inst_flashAddressCalc(
    .fontWidth(layerWidth),
    .fontHeight(layerHeight),
    .xOffset(layerX),
    .yOffset(layerY),
    .fontIndex(fontIndex),
    .addressOffsetBits(flashAddressBits)
);

// Assign registers based on header information
always begin
    // Determine if reading RAM and Flash is necessary
    if (currLayerHeader[0]) begin
        // Layer populated
        if (currLayerHeader[1]) begin
            // Sprite layer

            // Set text fields to 0
            readFlashEn <= 1'b0;

            // Read RAM only if sprite is on current pixel 
				// Because layer[XY] is unsigned, upper bounds check also is a layer[XY] > 0 check
            readRamEn <= 
                // Check X bounds (sprite width)
                layerX < $unsigned(currLayerHeader[31:16]) &&
					 // Check Y bounds (sprite height)
                layerY < $unsigned(currLayerHeader[47:32]);
        end else begin
            // Text layer

            // Read flash only if reading RAM
				// Because layer[XY] is unsigned, upper bounds check also is a layer[XY] > 0 check
            readFlashEn <= 
					 // Check X bounds (num chars * font width)
                layerX < $unsigned(currLayerHeader[31:16]) * $unsigned(currLayerHeader[111:96]) &&
                // Check Y bounds (font height)
                layerY < $unsigned(currLayerHeader[47:32]);

            // Read RAM to find character if text covers pixel
            readRamEn <= 
                // Check X bounds (num chars * font width)
                layerX < currLayerHeader[31:16] * currLayerHeader[111:96] &&
                // Check Y bounds (font height)
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

    // Find index of character to draw on current pixel if layer is text
	 
    characterIndex <= currLayerHeader[1] ? 0 : ($unsigned(layerX) / $unsigned(layerWidth));

    
end
endmodule