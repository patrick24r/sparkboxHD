module timingTester(
	input clk,
	input [15:0] data,
	output [15:0] muxOut,
	output [29:0] flashaddr,
	output [25:0] ramaddr,
	output readflashen,
	output readramen
);


/* */
reg [127:0] layerHeader;
wire [15:0] xout;
wire [15:0] yout;
wire [15:0] charout; 
wire [7:0] layerid;

assign muxOut = xout ^ yout ^ charout ^ layerid;

always @(posedge clk) begin
	layerHeader <= (layerHeader << 16) | data;
end


aluTop inst_aluTiming(
	.currLayerHeader(layerHeader), // Current layer header data
	.pixelX(11'd0), // Current X pixel position
	.pixelY(11'd0), // Current Y pixel position
   .readFlashEn(readflashen), // Enable read from flash for fonts
   .readRamEn(readramen), // Enable read from RAM for data
   .layerID(layerid), // ID number for the layer specified by the input 'layer'
   .ramAddressOffset(ramaddr), // Address offset for RAM (words)
   .flashAddressBits(flashaddr), // Address for flash (bits)
	.layerX(xout),
	.layerY(yout),
	.characterIndex(charout)
);

// */

endmodule
