module paletteTop(
    input clk_pipe, // Pipeline clock
    input clk_lcd, // LCD clock, 15 MHz max
    input rst,
    input writeEn,
    input [4:0] controllerLayer, // Selects layer for read/write
    input [4:0] controllerColor, // Selects color slot for read/write
    input controllerRGB, // Controller RGB select
    input [15:0] controllerWriteData, // Controller write data
    input [4:0] pipeLayer, // Layer for pipeline read
    input [4:0] pipeColor, // Color index for pipeline read
	input [10:0] xPosition, // Used to ensure no repeat pixel is found
	input [10:0] yPosition, // Used to ensure no repeat pixel is found
    output [15:0] controllerReadData, // Controller read data
    output [23:0] pipeReadData, // RGB 888 formatted color data for output
    output [7:0] red, // red lcd pins
	 output [7:0] green, // green lcd pins
	 output [7:0] blue, // blue lcd pins
	 output dclk, // Data clk lcd pin
	 output disp, // Display enable lcd pin
	 output hsync, // hsync lcd pin
	 output vsync, // vsync lcd pin
	 output den, // data enable lcd pn
    output [7:0] bufferSize, // Current size of pixel data buffer
    output bufferEmpty, // 1 = pixel buffer empty
    output bufferFull, // 1 = pixel buffer full
    output pixelFoundNew // 1 = non-transparent new x-y pixel found
);

// Determine if a found pixel has new X/Y coordinates
wire pixelFound;
wire [23:0] lcdReadData;
reg [10:0] prevX;
reg [10:0] prevY;
reg lcdNeedsData;

initial begin
    prevX = ~11'd0;
    prevY = ~11'd0;
end

assign pixelFoundNew = (pixelFound && (prevX != xPosition || prevY != yPosition));

// On the same edge we write to fifo buffer, save last x/y pair
// Update the pair only if
always @(negedge clk_pipe or negedge rst) begin
    if (!rst) begin
        prevX <= 0;
        prevY <= 0;
    end else begin
        if (pixelFoundNew) begin
            prevX <= xPosition;
            prevY <= yPosition;
        end
    end
end

// Palette memory has asynchronous reads,
// pipeline clock is used for writing
// NOTE: THIS MEANS THE PIPELINE CLOCK TO THIS BLOCK
// HAS TO BE ENABLED EVEN WHEN NOT RENDERING
paletteMemControl inst_paletteMemCtrl(
   clk_pipe,
   rst,
   writeEn,
   controllerLayer,
   controllerColor,
   controllerRGB,
   controllerWriteData,
   pipeLayer,
   pipeColor,
   controllerReadData,
   pipeReadData, // Data read for pipeline from palette memory
   pixelFound
);

// FIFO buffer that removes items on clk_pixelReq rising
// edges and adds items on clk_pipe falling edges if the pixel is found
// and is new (no repeat pixels if 2+ layers exist on same pixel)
paletteFifoBuffer inst_paletteBuffer(
    !clk_pipe,
    clk_lcd,
    rst,
	 lcdNeedsData, // Read enable
    pixelFoundNew, // Write enable
    pipeReadData, // Data read for pipeline from palette memory
    lcdReadData, // Data read for lcd from fifo buffer 
    bufferSize,
    bufferEmpty,
    bufferFull
);


// LCD Controller
lcdPixelWriter inst_lcdInterface(
	clk_lcd,
   rst,
	bufferEmpty, // 1 if buffer empty, no valid data
	lcdReadData, // Data from fifo buffer to LCD
   lcdNeedsData, // 1 if lcd needs valid data, 0 else
   // ALL THE FOLLOWING SIGNALS ARE OUTPUT SIGNALS TO THE LCD
	red, 
	green,
	blue,
	dclk,
	disp,
	hsync,
	vsync,
	den
);

endmodule