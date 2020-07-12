module pixelCounter 
#(parameter HOR_PIX = 'd480, // Screen width (pixels)
  parameter VER_PIX = 'd272, // Screen height (pixels)
  parameter NUM_LAYERS = 'd32, // Number of layers
  parameter X_DEPTH = $clog2(HOR_PIX), // Number of bits for x
  parameter Y_DEPTH = $clog2(VER_PIX),
  parameter LAYER_DEPTH = $clog2(NUM_LAYERS)
)(
	input clk, // clk
	input rst, // reset
	input pixelInc, // Force increment pixel
	output logic [LAYER_DEPTH-1:0] layer,
	output logic [X_DEPTH-1:0] x,
	output logic [Y_DEPTH-1:0] y
);
	
logic newX, newY;

// If x hits the limit or rolls over, indicate a new X
assign newX = (layer + 1 == NUM_LAYERS || layer + 1 == 0 || pixelInc);
// If a new x is coming, determine if we need a new y
assign newY = newX && (x + 1 == HOR_PIX || x + 1 == 0);

initial begin
	x = 'd0;
	y = 'd0;
end

	
always_ff @ (posedge clk, negedge rst) begin
	if (!rst) begin
		layer <= 'd0;
		x <= 'd0;
		y <= 'd0;
	end else begin
		// Update layer index
		if (newX)
			layer <= 0;
		else
			layer <= layer + 1;
		
		// Update x pixel position
		if (!newX)
			x <= x;
		else if (newY)
			x <= 0;
		else
			x <= x + 1;
			
		// Update y pixel position
		if (!newY)
			y <= y;
		else if (y + 1 == VER_PIX || y + 1 == 0) 
			y <= 0;
		else
			y <= y + 1;
		
	end
end	
	
endmodule