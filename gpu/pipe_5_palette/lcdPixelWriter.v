module lcdPixelWriter(
	input clk_12mhz,
	input bufferEmpty,
	input [23:0] rgb,
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
	STATE_PRERENDER = 'h0,
	STATE_RENDER = 'h1;
	

// Timing parameters
parameter 
	THBP = 'd43, // horizontal back porch
	HOR_PIX = 'd480, // screen width (pixels)
	THFP = 'd2, // horizontal front porch
	THW = 'd1, // hsync width (dclk cycles)
	TVBP = 'd12, // vertical back porch
	VER_PIX = 'd272, // screen height (pixels)
	TVFP = 'd1, // vertical front porch
	TVW = 'd1; // vsync width (hclk cycles)
	


reg state;

reg unsigned [15:0] dclk_counter;
reg unsigned [15:0] hclk_counter;

reg bufferWasEmpty;

// Initialize internal registers to prepare for a frame
initial begin
	state = STATE_PRERENDER;
	dclk_counter = 16'd0;
	hclk_counter = 16'd0;
	hsync = 1'd1;
	vsync = 1'd1;
	den = 1'd0;
	bufferWasEmpty = 1'd1;
end


// Always assign rgb data on output pins
assign red = rgb[23:16];
assign green = rgb[15:8];
assign blue = rgb[7:0];
// Always want the output clock to be the input clock unless
// the pixel buffer is empty or not rendering
assign dclk = (clk_12mhz && state == STATE_RENDER && !bufferWasEmpty);
assign disp = 1'b1;

// Inputs update on positive clk_12mhz
always @(clk_12mhz) begin
	if (clk_12mhz) begin
		// Positive edge
		case (state)
		
		STATE_PRERENDER: begin
			hsync <= 1'b1;
			vsync <= 1'b1;
			den <= 1'b0;
		end
		
		STATE_RENDER: begin
			hsync <= dclk_counter != THFP;
			vsync <= hclk_counter != TVFP;
			den <= (dclk_counter >= THFP + THBP && hclk_counter >= TVFP + TVBP);
		end
		
		endcase
		// Determine state updates on negative edge
		state <= state;
		bufferWasEmpty <= bufferWasEmpty;
		
		hclk_counter <= hclk_counter;
		dclk_counter <= dclk_counter;
	end else begin
		// Negative edge
		case (state)
		
		STATE_PRERENDER: begin
			dclk_counter <= 16'd0;
			hclk_counter <= 16'd0;
			// Determine state updates on negative edge
			if (!bufferEmpty) state <= STATE_RENDER;
			else state <= STATE_PRERENDER;
		end
		
		STATE_RENDER: begin
			// Determine if dclk counter is rolling over
			if (bufferEmpty) dclk_counter <= dclk_counter;
			else begin
				if (dclk_counter != THFP + HOR_PIX + THBP - 1'b1)
					dclk_counter <= dclk_counter + 1'b1;
				else dclk_counter <= 16'd0;	
			end
			// Determine if hclk counter is rolling over
			if (hclk_counter != TVFP + VER_PIX + TVBP - 1'b1) begin
				hclk_counter <= hclk_counter + 1'b1;
				state <= state;
			end else begin
				// Go to prerender when hclk_counter rolls over, done a frame
				hclk_counter <= 16'd0;
				state <= STATE_PRERENDER;
			end
			
		end
		
		endcase
		
		// Save buffer empty status
		// NOTE: this means rgb must update on clk_12mhz falling edges
		bufferWasEmpty <= bufferEmpty;
		// Following registers only updated on rising edges
		hsync <= hsync;
		vsync <= vsync;
		den <= den;
	end
end

endmodule