module addressCalculationUnit
#(
	parameter HOR_PIX = 'd480, // Screen width (pixels)
	parameter VER_PIX = 'd272 // Screen height (pixels)
)(
	input clk, // 50 MHz clock maximum
	input pipeline_clk_n, // Pipeline clock negated
	input [127:0] layerRegisters, // All registers for the 
	input unsigned [X_DEPTH:0] xPixel, // x pixel position
	input unsigned [Y_DEPTH:0] yPixel, // y pixel position
	output logic rdy, // 1 = ALU finished, ready for new pipeline data
	
	// Outputs used 
	output logic unsigned [15:0] layerX,
	output logic unsigned [15:0] layerY,
	
	// Outputs used to read memory
	output logic [26:0] ramAddressOffsetBytes,
	output logic [29:0] flashAddressOffsetBits
	
);
// This module calculates the address offset to read from RAM and Flash
localparam X_DEPTH = $clog2(HOR_PIX);
localparam Y_DEPTH = $clog2(VER_PIX);

logic readRamEn, readFlashEn, ramRdy, flashRdy;

always_comb begin
	// Find X and Y offset for this layer on this pixel
	layerX <= xPixel - layerRegisters[63:48];
	layerY <= yPixel - layerRegisters[79:64];

	// Find if we need to read from Flash - only for text layers
	// ---------------------------------------------------------------
	// layerRegisters[0]: 1 if layer is populated, 0 if layer is vacant
	// layerRegisters[1]: 1 if sprite layer, 0 if text layer
	// layerRegisters[2]: 1 if layer is visible, 0 if layer is invisible
	if (layerRegisters[1] || !(layerRegisters[0] && layerRegisters[2])) readFlashEn <= 0;
	else begin
		// Text layer, check if pixel is in bounds
		readFlashEn <= layerX < $unsigned(layerRegisters[31:16]) * 
			$unsigned(layerRegisters[111:96]) && // Check X bounds (Text length * Font width)
			layerY < $unsigned(layerRegisters[47:32]); // Check Y bounds (font height)
	end

	// Find if we need to read from RAM - For both sprites and text
	// ---------------------------------------------------------------
	// layerRegisters[0]: 1 if layer is populated, 0 if layer is vacant
	// layerRegisters[1]: 1 if sprite layer, 0 if text layer
	// layerRegisters[2]: 1 if layer is visible, 0 if layer is invisible
	if (!(layerRegisters[0] && layerRegisters[2])) readRamEn <= 0;
	else begin
		if (layerRegisters[1])
			// Sprite layer, check bounds
			readRamEn <= layerX < $unsigned(layerRegisters[31:16]) &&
				layerY < $unsigned(layerRegisters[47:32]);
		else
			// Text layer, only read from RAM if we need to read from flash
			readRamEn <= readFlashEn;
	end
	
end


// Determine if the entire unit is ready to advance
always_comb begin
	rdy <= 
		(readRamEn ? ramRdy : 1) && // RAM ready
		(readFlashEn ? flashRdy : 1); // Flash ready
end

ramAddressCalc inst_ramCalc(
	.clk,
	.rst(pipeline_clk_n), // reset the calculation for new pipe data
	.isSprite(layerRegisters[1]),
	.frameNumber(layerRegisters[127:120]),
	.height(layerRegisters[47:32]),
	.width(layerRegisters[31:16]),
	.layerX,
	.layerY,
	.rdy(ramRdy),
	.addressOffsetBytes(ramAddressOffsetBytes)
);

flashAddressCalc inst_flashAddr(
	.clk,
	.rst(pipeline_clk_n), // reset the calculation for new pipe data
	.drawnFontWidth(layerRegisters[31:16]),
	.drawnFontHeight(layerRegisters[47:32]),
	.layerX,
	.layerY,
	.fontSelectionIndex(layerRegisters[95:80]),
	.rdy(flashRdy),
	.addressOffsetBits(flashAddressOffsetBits)
);


endmodule