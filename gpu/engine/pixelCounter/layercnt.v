// This module keeps track of the current layer being processed
module layercnt(
	input layerInc, 
	input reset, 
	output reg [4:0] layer
);

// Initialize internal register to 0
initial layer = 0;

// On layerInc edge, increment counter
always@(posedge layerInc) begin
	// Reset if low
	if (!reset)
		layer <= 0;
	else 
		layer <= layer + 1;
end

endmodule