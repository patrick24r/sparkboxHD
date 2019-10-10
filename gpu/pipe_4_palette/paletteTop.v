//`include "paletteMem.v"

module paletteTop(
    input clk,
    input rst,
    input writeEn,
    input [4:0] controllerLayer, // Selects layer for read/write
    input [4:0] controllerColor, // Selects color slot for read/write
    input [1:0] controllerRGB, // Controller RGB select
    input [15:0] controllerWriteData, // Controller write data
    input [4:0] pipeLayer, // Layer for pipeline read
    input [4:0] pipeColor, // Color index for pipeline read
    output reg [15:0] controllerReadData, // Controller read data
    output [23:0] pipeReadData, // RGB 888 formatted color data for output
    output pixelFound // 1 = non-transparent pixel exists, 0 = transparent pixel exists
);

parameter colorR = 2'b00,
          colorG = 2'b01,
          colorB = 2'b10,
          colorX = 2'b11; // Reserved, not currently used


wire [47:0] allPaletteReads;

// A pixel is found as long as the color index is not all 0's
assign pixelFound = rst ? pipeColor != 0 : 1'b0;

// TODO: Update based on how HDMI wants colors formatted
// Currently RGB 888 (Using 8 MSB of 16 bit color storage)
assign pipeReadData = rst ? {allPaletteReads[47:40], allPaletteReads[31:24], allPaletteReads[15:8]} : {24{1'b0}};

// NOTE: The color position 0 is reserved for transparent. 
// Therefore any and all writes to it are disabled here.

// Red data palette memory
paletteMem inst_paletteMemR(
    clk,
    rst,
    writeEn && controllerRGB == colorR && controllerColor != 0,
    {controllerLayer, controllerColor},
    controllerWriteData,
    {pipeLayer, pipeColor},
    allPaletteReads[47:32]
);

// Green data palette memory
paletteMem inst_paletteMemG(
    clk,
    rst,
    writeEn && controllerRGB == colorG && controllerColor != 0,
    {controllerLayer, controllerColor},
    controllerWriteData,
    {pipeLayer, pipeColor},
    allPaletteReads[31:16]
);

// Blue data palette memory
paletteMem inst_paletteMemB(
    clk,
    rst,
    writeEn && controllerRGB == colorB && controllerColor != 0,
    {controllerLayer, controllerColor},
    controllerWriteData,
    {pipeLayer, pipeColor},
    allPaletteReads[15:0]
);

// MUX for controller reading back color data
always begin
    if (!rst) begin
		  controllerReadData <= 16'd0;
    end
    else begin
        case (controllerRGB)
            colorR: controllerReadData <= allPaletteReads[47:32];
            colorG: controllerReadData <= allPaletteReads[31:16];
            colorB: controllerReadData <= allPaletteReads[15:0];
            colorX: controllerReadData <= 16'd0;
        endcase
    end
end

endmodule