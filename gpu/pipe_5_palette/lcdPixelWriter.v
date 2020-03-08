module lcdPixelWriter(
    input clk_12mhz,
    input rst,
    input invalidData, // 1 = invalid rgb data, 0 = valid rgb data
    input [23:0] rgb,
    output rgbRequest, // Determines whether or not valid rgb data is needed
    output reg [7:0] red,
    output reg [7:0] green,
    output reg [7:0] blue,
    output dclk, // output clock
    output disp, // display enable
    output reg hsync, // horizontal sync
    output reg vsync, // vertical sync
    output den // data enable pin
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

reg dataWasInvalid;

// Initialize internal registers to prepare for a frame
initial begin
    state = STATE_PRERENDER;
    dclk_counter = 16'd0;
    hclk_counter = 16'd0;
    x = 11'd0;
    y = 11'd0;
    hsync = 1'd1;
    vsync = 1'd1;
    dataWasInvalid = 1'd0;
end


// Always assign rgb data on output pins
assign red = rgb[23:16];
assign green = rgb[15:8];
assign blue = rgb[7:0];
// Always want the output clock to be the input clock unless
// the pixel buffer is empty or not rendering
assign dclk = (clk_12mhz && state == STATE_RENDER && !dataWasInvalid);
assign disp = 1'b1;
// 1 if new rgb data is needed, 0 otherwise
// Because dclk and hclk counters update on negative clock edges,
// so does the rgbRequest output
assign rgbRequest = (dclk_counter >= THFP + THBP && hclk_counter >= TVFP + TVBP);


// Inputs update on positive clk_12mhz
always @(clk_12mhz or negedge rst) begin
    if (!rst) begin
        hsync <= 1'b1;
        vsync <= 1'b1;
        den <= 1'b0;
        state <= STATE_PRERENDER;
        hclk_counter <= 1'b0;
        dclk_counter <= 1'b0;
    end else begin
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
                if (bufferEmpty)
                    dclk_counter <= dclk_counter;
                else begin
                    if (dclk_counter != THFP + HOR_PIX + THBP - 1'b1)
                        dclk_counter <= dclk_counter + 1'b1;
                    else dclk_counter <= 16'd0;	
                end
                // Determine if hclk counter is rolling over
                if (hclk_counter != TVFP + VER_PIX + TVBP - 1'b1) 
                    hclk_counter <= hclk_counter + 1'b1;
                    state <= state;
                else begin
                    // Go to prerender when hclk_counter rolls over, done a frame
                    hclk_counter <= 16'd0;
                    state <= STATE_PRERENDER;
                end
                
            end
            
            endcase
            
            // Save buffer empty status
            // NOTE: this means rgb must update on clk_12mhz falling edges
            dataWasInvalid <= invalidData;
            // Following registers only updated on rising edges
            hsync <= hsync;
            vsync <= vsync;
            den <= den;
        end
    end
end

endmodule