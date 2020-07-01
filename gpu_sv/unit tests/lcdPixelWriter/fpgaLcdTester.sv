module fpgaLcdTester(
	input CLOCK_50, // 50 MHz clock
	inout [33:0] GPIO_0, // GPIO 0 headers
	inout [33:0] GPIO_1 // GPIO 1 headers
);

parameter
	SCREEN_COLOR = 24'h0000ff,
	CLOCK_DIV = 2;

logic unsigned [7:0] clock_counter;
logic clock_adj;

lcdBus out();

assign GPIO_0[7:0] = out.rgb[23:16]; // Red pins
assign GPIO_0[23:16] = out.rgb[15:8]; // Green pins
assign GPIO_0[15:8] = out.rgb[7:0]; // Blue pins
assign GPIO_0[24] = out.d_clk; // LCD clk pin
assign GPIO_0[25] = out.disp_en; // Display enable pin
assign GPIO_0[26] = out.hsync; // hsync pin
assign GPIO_0[27] = out.vsync; // vsync pin
assign GPIO_0[28] = out.d_en; // data enable pin

always @(posedge CLOCK_50) begin
	if (clock_counter == CLOCK_DIV - 1'b1) begin
		clock_counter <= 'd0;
		clock_adj <= !clock_adj;
	end else begin
		clock_counter <= clock_counter + 'd1;
		clock_adj <= clock_adj;
	end
end


lcdPixelWriter inst_lcdPixelWriter(
	.clk_12mhz(clock_adj), // 15 MHz clock
	.rst(1'b1), // never reset
	.rgb(SCREEN_COLOR), // RGB input
	.data_valid(1'b1), // Data always valid for this test
	.data_req(GPIO_1[32]), // rgb color request
	.lcdPins(out.controller)
);



endmodule