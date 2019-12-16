module paletteTop(
    input clk_pipe, // Pipeline clock
	input clk_pixelReq, // Pixel request pulse (12 Mhz)
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
    output pixelFoundNew // 1 = non-transparent pixel exists, 0 = transparent pixel exists
);

// Determine if a found pixel has new X/Y coordinates
wire pixelFound;
wire [23:0] hdmiReadData;
reg [10:0] prevX;
reg [10:0] prevY;

initial begin
    prevX = ~11'd0;
    prevY = ~11'd0;
end

assign pixelFoundNew = (pixelFound && (prevX != xPosition || prevY != yPosition));

// At next positive edge, save last x/y pair
always @(posedge clk_pipe) begin
    if (pixelFoundNew) begin
        prevX <= xPosition;
        prevY <= yPosition;
    end
end

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
   pipeReadData,
   pixelFound
);

// FIFO buffer that removes items on clk_pixelReq rising
// edges and adds items on clk_pipe falling edges if the pixel is found
// and is new (no repeat pixels if 2+ layers exist on same pixel)
paletteFifoBuffer inst_paletteBuffer(
    !clk_pipe,
    clk_pixelReq,
    rst,
    pixelFoundNew,
    pipeReadData,
    hdmiReadData,
    bufferSize,
    bufferEmpty,
    bufferFull
);


// Uses fifo buffer output to control 
lcdPixelWriter inst_lcdInterface(
	!clk_pixelReq,
	bufferEmpty,
	hdmiReadData,
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