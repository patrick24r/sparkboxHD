module layerHeaderCtrl(
    input [15:0] command, // Current command
    output rstLayerheaders, // Reset all layer headers
    output rstSingleLayer, // Reset 1 layer header
    output [2:0] layerRegister, // Index of layer header register for r/w/rst
    output wLayerHeaders // write layer headers enable bit
);

// Reset all layers on reset all gpu memories command or on reset all layer headers command
assign rstLayerheaders = !(command[15:11] == 5'b11000 || (command[15:11] == 5'b11001 && command[5]));
// Reset 1 layer command
assign rstSingleLayer = !(command[15:11] == 5'b11001 == 0 && !command[5]);
// Layer register select
assign layerRegister = command[8:6]; 
// Write layer headers
assign wLayerHeaders = (command[15:11] == 5'b10001); 

endmodule