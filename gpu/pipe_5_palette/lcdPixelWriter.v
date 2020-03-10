module lcdPixelWriter(
	input clk_12mhz,
	input rst,
	input invalidData,
	input [23:0] rgb,
	output reg rgbRequest, // Request for rgb data
	output [7:0] red,
	output [7:0] green,
	output [7:0] blue,
	output dclk, // output clock
	output disp, // display enable
	output reg hsync, // horizontal sync
	output reg vsync, // vertical sync
	output reg den // data enable pin
);

/* NOTES: hsync and vsync hardware pins are pulled to vcc, with
 * default register values
 * VDPOL = 1, HDPOL = 1;
*/


parameter 
	STATE_PRERENDER = 1'h0,
	STATE_RENDER = 1'h1;
	

// Timing parameters
parameter 
	THBP = 'd1, // horizontal back porch
	HOR_PIX = 'd480, // screen width (pixels)
	THFP = 'd2, // horizontal front porch
	THW = 'd1, // hsync width (dclk cycles)
	TVBP = 'd1, // 'd12, // vertical back porch
	VER_PIX = 'd272, // screen height (pixels)
	TVFP = 'd1, // vertical front porch
	TVW = 'd1; // vsync width (hclk cycles)
	


reg state;

reg unsigned [15:0] dclk_counter;
reg unsigned [15:0] hclk_counter;

reg dclk_en;

// Initialize internal registers to prepare for a frame
initial begin
	state = STATE_PRERENDER;
	dclk_counter = 16'd0;
	hclk_counter = 16'd0;
	hsync = 1'd1;
	vsync = 1'd1;
	den = 1'd0;
end


// Always assign rgb data on output pins
assign red = rgb[23:16];
assign green = rgb[15:8];
assign blue = rgb[7:0];
assign dclk = clk_12mhz && dclk_en;
assign disp = 1'b1;

// Inputs update on negative clk_12mhz
always @(negedge clk_12mhz or posedge invalidData) begin
	// Always want the output clock to be the input clock unless
	// the pixel buffer is empty or not rendering
	dclk_en <= (state == STATE_RENDER && !invalidData);
	rgbRequest <= (dclk_counter >= THFP + THBP && hclk_counter >= TVFP + TVBP);
end

// Outputs update on positive clk_12mhz
always @(posedge clk_12mhz or negedge rst) begin
	if (!rst) begin
		hsync <= 1'b1;
		vsync <= 1'b1;
		den <= 1'b0;
		dclk_counter <= 16'd0;
		hclk_counter <= 16'd0;
		state <= STATE_PRERENDER;
	end else
	// Positive edge (hsync, vsync, den)
	case (state)
		
	STATE_PRERENDER: begin
		hsync <= 1'b1;
		vsync <= 1'b1;
		den <= 1'b0;
		
		dclk_counter <= 16'd0;
		hclk_counter <= 16'd0;
		// Determine state update
		if (!invalidData) state <= STATE_RENDER;
		else state <= STATE_PRERENDER;
	end
		
	STATE_RENDER: begin
		hsync <= dclk_counter != THFP;
		vsync <= hclk_counter != TVFP;
		den <= rgbRequest;
		
		// Determine if dclk counter is rolling over
		if (invalidData) dclk_counter <= dclk_counter;
		else begin
			if (dclk_counter != THFP + HOR_PIX + THBP - 1'b1)
				dclk_counter <= dclk_counter + 1'b1;
			else dclk_counter <= 16'd0;	
		end
		// Increment hclk_counter on dclk_counter rolling over
		if (dclk_counter != THFP + HOR_PIX + THBP - 1'b1) hclk_counter <= hclk_counter;
		else begin
			if (hclk_counter != TVFP + VER_PIX + TVBP - 1'b1) begin
				hclk_counter <= hclk_counter + 1'b1;
				state <= state;
			end else begin
				// Go to prerender when hclk_counter rolls over, done a frame
				hclk_counter <= 16'd0;
				state <= STATE_PRERENDER;
			end
		end
	end
		
	default: begin
		hsync <= 1'b1;
		vsync <= 1'b1;
		den <= 1'b0;
		dclk_counter <= 16'd0;
		hclk_counter <= 16'd0;
		state <= STATE_PRERENDER;
	end
		
	endcase
end

endmodule