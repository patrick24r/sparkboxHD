module aluSim(
	input clk,
	input pipeline_clk,
	
	
	input unsigned [4:0] xPixel, // Currently rendering x pixel
	input unsigned [4:0] yPixel, // Currently rendering y pixel
	
	
	// Layer registers
	input isPopulated,
	input isSprite,
	input unsigned [4:0] xPosition, // Layer top left pixel x pos
	input unsigned [4:0] yPosition, // Layer top left pixel y pos
	input unsigned [4:0] width, // Sprite/font width
	input unsigned [4:0] height, // Sprite/font height
	input unsigned [3:0] numberOfCharacters,
	
	// Outputs used 
	output rdy,
	output logic unsigned [15:0] layerX,
	output logic unsigned [15:0] layerY,
	
	// Outputs used to read memory
	output logic [26:0] ramAddressOffsetBytes,
	output logic [29:0] flashAddressOffsetBits
);



logic [127:0] registers;
initial begin
	registers = 0;
end


always_comb begin
	registers[0] <= isPopulated;
	registers[1] <= isSprite;
	registers[2] <= isPopulated;
	
	registers[52:48] <= xPosition;
	registers[68:64] <= yPosition;
	registers[20:16] <= width;
	registers[36:32] <= height;
	registers[99:96] <= numberOfCharacters;
end


addressCalculationUnit test_alu(
	.clk,
	.pipeline_clk_n(!pipeline_clk),
	.layerRegisters(registers),
	.xPixel({4'd0,xPixel}),
	.yPixel({4'd0,yPixel}),
	.rdy,
	.layerX,
	.layerY,
	.ramAddressOffsetBytes,
	.flashAddressOffsetBits
);


endmodule