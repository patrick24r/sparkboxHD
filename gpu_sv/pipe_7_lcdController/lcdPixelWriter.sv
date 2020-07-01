module lcdPixelWriter
#(parameter HOR_PIX = 'd480, // Screen width (pixels)
  parameter VER_PIX = 'd272, // Screen height (pixels)
  parameter THBP = 'd1, // Horizontal back porch
  parameter THFP = 'd2, // Horizontal front porch
  parameter THW = 'd1, // hsync width (dclk cycles)
  parameter TVBP = 'd1, // Vertical back porch
  parameter TVFP = 'd1, // vertical front porch
  parameter TVW = 'd1 // vsync width (hclk cycles)
)(
	input clk_12mhz,
	input rst,
	input [23:0] rgb,
	input data_valid, // Determine if the current input rgn
	output data_req, // Ready for next pixel data
	lcdBus.controller lcdPins
);

// States
typedef enum {
	STATE_PRERENDER = 0,
	STATE_RENDER = 1
} lcd_states;
	
	
	
lcd_states state;
logic unsigned [15:0] dclk_counter;
logic unsigned [15:0] hclk_counter;
logic dclk_en;


initial begin
	state = STATE_PRERENDER;
	dclk_counter = 0;
	hclk_counter = 0;
	lcdPins.hsync = 1;
	lcdPins.vsync = 1;
	lcdPins.d_en = 0;
end

// The clk to the lcd is the same 12 MHz clock, only gated
assign lcdPins.d_clk = clk_12mhz && dclk_en;
// For now, no reason to turn off the display
assign lcdPins.disp_en = 1'b1;
assign lcdPins.rgb = rgb;

// Clock gating and data requests update on falling edge
always_ff @(negedge clk_12mhz) begin
	// Always want the output clock to be the input clock unless
	// the data is not valid or not rendering
	dclk_en <= (state == STATE_RENDER && data_valid);
	data_req <= (dclk_counter >= THFP + THBP && hclk_counter >= TVFP + TVBP);
end

// Outputs update on positive clk_12mhz
always_ff @(posedge clk_12mhz or negedge rst) begin
	if (!rst) begin
		lcdPins.hsync <= 1;
		lcdPins.vsync <= 1;
		lcdPins.d_en <= 0;
		dclk_counter <= 0;
		hclk_counter <= 0;
		state <= STATE_PRERENDER;
	end else
	
	case (state)
		
	STATE_PRERENDER: begin
		lcdPins.hsync <= 1;
		lcdPins.vsync <= 1;
		lcdPins.d_en <= 0;
		
		dclk_counter <= 0;
		hclk_counter <= 0;
		// Determine state update
		if (data_valid) state <= STATE_RENDER;
		else state <= STATE_PRERENDER;
	end
		
	STATE_RENDER: begin
		lcdPins.hsync <= dclk_counter != THFP;
		lcdPins.vsync <= hclk_counter != TVFP;
		lcdPins.d_en <= data_req;
		
		// Determine if dclk counter is rolling over
		if (data_valid) begin
			if (dclk_counter != THFP + HOR_PIX + THBP - 1'b1)
				dclk_counter <= dclk_counter + 1'b1;
			else dclk_counter <= 16'd0;	
		end else begin
			dclk_counter <= dclk_counter;
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
		lcdPins.hsync <= 1'b1;
		lcdPins.vsync <= 1'b1;
		lcdPins.d_en <= 1'b0;
		dclk_counter <= 16'd0;
		hclk_counter <= 16'd0;
		state <= STATE_PRERENDER;
	end
		
	endcase
end
	
endmodule