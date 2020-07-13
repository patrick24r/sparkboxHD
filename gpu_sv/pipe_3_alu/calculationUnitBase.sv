module calculationUnitBase
#(
	parameter CYCLES_TO_COMPLETE = 4,
	parameter CALCULATION_WIDTH = 27
)(
	input clk,
	input rst,
	input unsigned [CALCULATION_WIDTH-1:0] calculation_i,
	output logic rdy,
	output logic unsigned [CALCULATION_WIDTH-1:0] calculation_o
);
// This module is used by the calculation units to properly handle multi-cycle
// calculations
logic [$clog2(CYCLES_TO_COMPLETE):0] cycleCounter;

initial begin
	cycleCounter = 1;
end

// Update the cycle counter. It's held at the maximum value until a calculation is requested.
// It then counts up to the correct cycle amount
always_ff @(posedge clk or negedge rst) begin
	if (!rst) begin
		// On reset, go back to 1st cycle
		cycleCounter <= 1;
	end else begin
		if (cycleCounter == CYCLES_TO_COMPLETE) begin
			cycleCounter <= CYCLES_TO_COMPLETE;
		end else begin
			cycleCounter <= cycleCounter + 1;
		end
	end
end


// Only output the input if the input is ready (stable) after the clock cycles are complete
always_comb begin
	if (cycleCounter == CYCLES_TO_COMPLETE) begin
		calculation_o <= calculation_i;
	end else begin
		calculation_o <= 0;
	end
	
	// Ready for a new calculation if currently on the last cycle
	rdy <= rst ? cycleCounter == CYCLES_TO_COMPLETE : 0;
	
end

endmodule