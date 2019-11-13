module paletteTop(
    input clk_pipe, // Pipeline clock
	 input clk_pixelReq, // Pixel request pulse for hdmi
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
    output pixelFound // 1 = non-transparent pixel exists, 0 = transparent pixel exists
);

// wire [23:0] colorValue;


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

// TODO: Include FIFO buffer that removes items on clk_pixelReq rising
// edges and adds items on clk_pipe falling edges if the pixel is found
// and is new (no repeat pixels if 2+ layers exist on same pixel)




endmodule