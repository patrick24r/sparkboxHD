module pinTester(
	input CLOCK_50,
	output reg [7:0] LED
);

initial begin
	clk_div = 'd0;
	LED = 'd0;
end

reg unsigned [31:0] clk_div;


always @(posedge CLOCK_50) begin
	if (clk_div < 2000000) begin
		clk_div <= clk_div + 1;
		LED <= LED;
	end else begin
		clk_div <= 0;
		LED <= LED + 1;
	end
end

endmodule