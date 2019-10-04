`include "commandBuffer.v"
`include "commandInterface.v"

module commandControlTop(
    input clk,
    input rst,
    input gpuBusy,
    input chipSelect, // Chip Select I pin
    input outputEnable, // Output enable I pin
    input commandClk, // Command Clock I pin
    input [15:0] inputCommand, // Command I pins
    input [15:0] dataFromGpu, // Data from GPU for chip reads
    inout [15:0] dataInOut, // Data I/O pins
    output [15:0] gpuCommand, // Command to GPU
    output [15:0] gpuData // Data to GPU 
);

// Intermediary wires between interface and buffer
wire [15:0] gpuCommandInt;
wire [15:0] gpuDataInt;


// Instantiate I/O level command interface
commandInterface inst_commandInterface(
    chipSelect,
    outputEnable,
    commandClk,
    inputCommand,
    datafromGpu,
    dataInOut,
    gpuCommandInt,
    gpuDataInt
);

// Instantiate active/smart command buffer
commandBuffer inst_commandBuffer(
    clk,
    rst,
    gpuBusy,
    gpuCommandInt,
    gpuDataInt,
    gpuCommand,
    gpuData
);



endmodule