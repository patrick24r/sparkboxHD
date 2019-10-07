//`include "commandBuffer.v"
//`include "commandInterface.v"

// NOTE: Pipeline clock used to ensure only 1 command executes per stage of pipeline
// We DO NOT want multiple commands executing while the pipeline is unmoving

module commandControlTop(
    input pipelineClk, // Pipeline clock
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

/* **************************************************************************
 *                              COMMAND USAGE
 * **************************************************************************
 * Commands are 16 bits [15:0] and are accompanied by 16 bit data
 * command[15:14] - Command Type
 *      00 - Reset
 *      01 - Read
 *      10 - Write
 *      11 - Reserved
 *          (potentially used to dynamically change size in
 *          RAM/Flash of each sprite/font)
 *          (Use to manually trigger frame updates and set frame rate)
 *
 *
 * command[13:11] - Target GPU memory
 *      000 - All GPU memories
 *      001 - layer headers
 *      010 - RAM (layer data storage)
 *      011 - Palette
 *      100 - Flash (text glyph data)
 *      101 - Reserved
 *      11X - Reserved
 *
 *
 * COMMAND PARAMETERS
 * If targeting pallete or layer headers memory
 * --------------------------------------------------
 * command[5:0]
 *      000000 - Layer 0
 *      000001 - Layer 1
 *      ...
 *      ...
 *      011111 - Layer 31
 *      1xxxxx - All layers
 *
 * If targeting layer headers
 * ------------------------------------------------
 * command[8:6] - Layer header register address
 * command[10:9] - Reserved
 *
 * If targeting palette
 * ------------------------------------------------
 * command[10:6] - Palette index
 *
 * If targeting RAM
 * ------------------------------------------------
 * command[10:3] - Layer ID
 * command[2:0] - Reserved
 *
 *
 * If targeting Flash memory (Should not be done by normal game)
 * ---------------------------------------------------
 * command[10:0] - Font index (2^11 theoretical possible fonts)
 * Only target flash if you really know what you're doing
 *
 * Each font takes up a fixed number of bytes in flash memory
 * Therefore only an index number is needed to determine the start write address
 *
 *
 * Notes: NOT all combinations of command pieces are supported
 * For example, reading/writing all gpu memories is not supported
 * Block reads/writes: RAM and Flash memories use block reads/writes
 * For block writes, continuously clock in the same write command (used to find start address)
 * and update data line bit values
 * For block reads, continuously clock in the same read command (used to find start address)
 * and data lines will update with read data
 *
 */


// Intermediary wires between interface and buffer
wire [15:0] gpuCommandInt;
wire [15:0] gpuDataInt;


// Instantiate I/O level command interface
commandInterface inst_commandInterface(
    chipSelect,
    outputEnable,
    commandClk,
    inputCommand,
    dataFromGpu,
    dataInOut,
    gpuCommandInt,
    gpuDataInt
);

// Instantiate active/smart command buffer
commandBuffer inst_commandBuffer(
    pipelineClk,
    rst,
    gpuBusy,
    gpuCommandInt,
    gpuDataInt,
    gpuCommand,
    gpuData
);



endmodule