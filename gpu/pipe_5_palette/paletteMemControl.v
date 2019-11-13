module paletteMemControl(
    input clk,
    input rst,
    input writeEn,
    input [4:0] controllerLayer, // Selects layer for read/write
    input [4:0] controllerColor, // Selects color slot for read/write
    input controllerRGB, // Controller RGB select
    input [15:0] controllerWriteData, // Controller write data
    input [4:0] pipeLayer, // Layer for pipeline read
    input [4:0] pipeColor, // Color index for pipeline read
    output reg [15:0] controllerReadData, // Controller read data
    output [23:0] pipeReadData, // RGB 888 formatted color data for output
    output pixelFound // 1 = non-transparent pixel exists, 0 = transparent pixel exists
);

parameter colorRG = 1'b1,
          colorBX = 1'b0;


wire [31:0] allPaletteReads;

// A pixel is found as long as the color index is not all 0's
assign pixelFound = rst ? pipeColor != 0 : 1'b0;

// TODO: Update based on how HDMI wants colors formatted
// Currently RGB 888 (Using 8 MSB of 16 bit color storage)
assign pipeReadData = rst ? allPaletteReads[31:8] : {24{1'b0}};

// NOTE: The color position 0 is reserved for transparent. 
// Therefore any and all writes to it are disabled here.

// Red and Green data palette memory
paletteMem inst_paletteMemRG(
    !clk, // Write on negedges to write in a single cycle
    rst,
    writeEn && controllerRGB == colorRG && controllerColor != 0,
    {controllerLayer, controllerColor},
    controllerWriteData,
    {pipeLayer, pipeColor},
    allPaletteReads[31:16]
);

// Blue data palette memory
paletteMem inst_paletteMemBX(
    !clk, // Write on negedges to write in a single cycle
    rst,
    writeEn && controllerRGB == colorBX && controllerColor != 0,
    {controllerLayer, controllerColor},
    controllerWriteData,
    {pipeLayer, pipeColor},
    allPaletteReads[15:0]
);

// MUX for controller reading back color data
always begin
    if (!rst) controllerReadData <= 16'd0;
    else begin
        case (controllerRGB)
            colorRG: controllerReadData <= allPaletteReads[31:16];
            colorBX: controllerReadData <= allPaletteReads[15:0];
        endcase
    end
end

endmodule