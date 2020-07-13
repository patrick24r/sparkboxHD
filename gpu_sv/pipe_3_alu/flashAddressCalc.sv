module flashAddressCalc
#(
	parameter memFontHeight = 128, // Height of character in flash memory (pixels). Must be a power of 2
	parameter memFontWidth = 64, // Width of characters in flash memory (pixels). Must be a power of 2
	parameter charactersPerFont = 256, // Number of characters per font in flash memory. Must be a power of 2
	parameter CYCLES_TO_COMPLETE = 6 // Number of clock cycles to fully complete a calculation
)(
	input clk,
	input rst,
	input unsigned [15:0] drawnFontWidth, // Font width as drawn on screen
	input unsigned [15:0] drawnFontHeight, // Font height as drawn on screen
	input unsigned [15:0] layerX,
	input unsigned [15:0] layerY,
	input unsigned [15:0] fontSelectionIndex,
	output logic rdy,
	output logic unsigned [29:0] addressOffsetBits
);

localparam BITS_HEIGHT_SHIFT = $clog2(memFontHeight);
localparam BITS_WIDTH_SHIFT = $clog2(memFontWidth);
localparam BITS_FONT_SHIFT = $clog2(charactersPerFont) + BITS_HEIGHT_SHIFT + BITS_WIDTH_SHIFT;

logic unsigned [29:0] addressCalcBits;

// NOTE: THIS CALCULATION DOES NOT INCLUDE CHARACTER OFFSET. CHARACTER OFFSET CANNOT BE ADDED
// UNITL THE CHARACTER VALUE IS KNOWN, AFTER READING FROM RAM
//
// Calculation:
// addressOffset = fontOffset + characterOffset + yOffset + xOffset
//
// ------------------------------------------------------------
// bitsPerCharacter = memFontHeight * memFontWidth
// bitsPerFont =  charactersPerFont * bitsPerCharacter
//
// ------------------------------------------------------------
// fontOffset = fontSelectionIndex * bitsPerFont
// characterOffset = characterIndex * bitsPerCharacter
// xOffset = (layerX % drawnFontWidth) / (drawnFontWidth-1) * (memFontWidth-1)
// yOffset = layerY / (drawnFontHeight-1) * (memFontHeight-1) * memFontWidth
// NOTE: characterIndex is not known yet
// ---------------------------------------------------------------------
always_comb begin
	addressCalcBits <= 
		(fontSelectionIndex << BITS_FONT_SHIFT) + // fontOffset
		(((layerX % drawnFontWidth) * (memFontWidth-1)) / (drawnFontWidth - 1))  + // xOffset
		((layerY * (memFontHeight-1) << BITS_WIDTH_SHIFT) / (drawnFontHeight-1)); // yOffset	
end


// Instantiate the cycle counter and output control module
calculationUnitBase #(.CYCLES_TO_COMPLETE(CYCLES_TO_COMPLETE), .CALCULATION_WIDTH(30)) inst_ramCalcBase(
	.clk,
	.rst,
	.calculation_i(addressCalcBits),
	.rdy,
	.calculation_o(addressOffsetBits)
);

endmodule