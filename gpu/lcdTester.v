module lcdTester(
	input CLOCK_50, // 50 MHz clock
	output [33:0] GPIO_0, // GPIO 0 headers
	output [33:0] GPIO_1 // GPIO 1 headers
);

parameter
	SCREEN_COLOR = 24'hdea2d7,
	CLOCK_DIV = 2;

reg unsigned [7:0] clock_counter;
reg clock_adj;

reg unsigned [23:0] color;

always @(posedge CLOCK_50) begin
	if (clock_counter == CLOCK_DIV - 1'b1) begin
		clock_counter <= 'd0;
		clock_adj <= !clock_adj;
	end else begin
		clock_counter <= clock_counter + 1;
		clock_adj <= clock_adj;
	end

end


lcdPixelWriter inst_lcdPixelWriter(
	clock_adj, // 15 MHz clock
	1'b1, // never reset
	1'b0, // Buffer never empty, for test
	SCREEN_COLOR, // RGB input
	GPIO_1[32], // rgb color request
	GPIO_0[7:0], // Red[7:0]
	GPIO_0[23:16], // Green[7:0]
	GPIO_0[15:8], // Blue[7:0]
	GPIO_0[24], // Output clock
	GPIO_0[25], // Display enable
	GPIO_0[26], // hsync
	GPIO_0[27], // vsync
	GPIO_0[28] // data enable
);

endmodule