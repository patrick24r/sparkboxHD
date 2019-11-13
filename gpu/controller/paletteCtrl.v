module paletteCtrl(
    input [15:0] command, // ccurrent command
    output rstPalette, // reset palette
    output wPalette, // write palette enable
    output [4:0] controllerColor, // color slot select
    output controllerRGB // rgb select
);

// Reset palette
assign rstPalette = !(command[15:11] == 5'b11011 || command[15:11] == 5'b11000);
// Write palette enable
assign wPalette = (command[15:11] == 5'b10011);
// Controller color slot select
assign controllerColor = command[10:7];
// Controller RGB select (RG or BX)
assign controllerRGB = command[6];

endmodule