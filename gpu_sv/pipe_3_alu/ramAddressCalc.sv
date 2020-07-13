module ramAddressCalc
#(
	parameter CYCLES_TO_COMPLETE = 4 // Number of clock cycles to complete a calculation
)(
	input clk,
	input rst,
	input isSprite, // 1 = sprite layer, 0 = text layer
	input unsigned [7:0] frameNumber,
	input unsigned [15:0] height,
	input unsigned [15:0] width,
	input unsigned [15:0] layerX, // X positon relative to the current layer
	input unsigned [15:0] layerY, // Y position relative to the current layer
	output logic rdy, // 1 = ready, 0 = busy
	output logic unsigned [26:0] addressOffsetBytes // RAM address offset in bytes
);

logic unsigned [26:0] addressCalcBytes;

// Calculate the address offset in bytes
always_comb begin
	if (isSprite) begin
		// Calculate the pixel offset, then convert to bytes (2 bytes / 1 pixel)
		addressCalcBytes <= ((frameNumber * height * width) + // Frame offset (pixels)
			(layerY * width) + layerX) // Pixel offset (pixels)
			<< 1; // 2 pixels per byte
	end else begin
		// Calculate which character in the text string on which the current pixel is
		addressCalcBytes <= layerX / width;
	end
end

// Instantiate the cycle counter and output control module
calculationUnitBase #(.CYCLES_TO_COMPLETE(CYCLES_TO_COMPLETE), .CALCULATION_WIDTH(27)) inst_ramCalcBase(
	.clk,
	.rst,
	.calculation_i(addressCalcBytes),
	.rdy,
	.calculation_o(addressOffsetBytes)
);

endmodule