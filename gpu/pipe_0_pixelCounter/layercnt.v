// This module keeps track of the current layer being processed
module layercnt(
	input layerInc,
	input reset,
	output reg overflow,
	output reg unsigned [4:0] layer
);

// Initialize internal register to 0
initial layer = 0;
initial overflow = 0;

// On layerInc edge, increment counter
always@(posedge layerInc or negedge reset) begin
	// Set to 0 on reset or pixel increment
	if (!reset) begin
		layer <= 0;
		overflow <= 0;
	end else begin 
		layer <= layer + 1;
		if (layer == 5'b11111) overflow <= 1;
		else overflow <= 0;
	end
end

endmodule