// NOTE: Pipeline clock used to ensure only 1 command executes per stage of pipeline
// We DO NOT want multiple commands executing while the pipeline is unmoving
// as that could lead to corrupted or missed commands

module commandControlTop(
    input gpuClk, // 400 MHz clock
    input rst,
    input gpuBusyController, // Controller busy signal
    input frameRendering, // Controller frame rendering signal
    input outputEnable, // Output enable I pin
    input commandClk, // Command Clock I pin
    input [15:0] dataFromGpu, // Data from GPU for chip reads
    inout [15:0] dataInOut, // Data I/O pins
    output readyBusy, // RDY/#BSY O pin, 1 = ready, 0 = busy
    output [15:0] gpuCommand, // Command to GPU
    output [15:0] gpuData // Data to GPU 
);

/* **************************************************************************
 *                              COMMAND USAGE
 * **************************************************************************
 * Commands are 16 bits [15:0] and are accompanied by 16 bit data
 * command[15:14] - Command Type
 *      00 - Special
 *      01 - Read
 *      10 - Write
 *      11 - Reset
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
 * If targeting layer headers
 * --------------------------------------------------
 * command[10:9] - Reserved
 * command[8:6] - Layer header register address
 * command[5:0]
 *      000000 - Layer 0
 *      000001 - Layer 1
 *      ...
 *      ...
 *      011111 - Layer 31
 *      1XXXXX - All Layers (Only valid for reset commands)
 *
 * If targeting palette
 * ------------------------------------------------
 * command[10:6] - Palette index / Color slot
 * command [5] - RGB specifier 
 *      0 - Blue and X (Maybe alpha at a later date?)
 *      1 - Red and Green
 * command[4:0]
 *      00000 - Layer 0
 *      00001 - Layer 1
 *      ...
 *      ...
 *      11111 - Layer 31
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
 * Special commands:
 * command[13:0]
 * 14'b00000000000000 - no op
 * 14'b00000000000001 - Update frame
 *
 * Notes: NOT all combinations of command pieces are supported
 * For example, reading/writing all gpu memories is not supported
 * Block reads/writes: RAM and Flash memories use block reads/writes
 * For block writes, continuously clock in the same write command
 * and update data line bit values
 * For block reads, continuously clock in the same read command
 * and data lines will update with read data
 *
 */


// Intermediary wires between interface and buffer
wire [15:0] gpuCommandInt;
wire [15:0] gpuDataInt;

// Instantiate I/O level command interface
commandInterface inst_commandInterface(
    outputEnable,
    commandClk,
    dataFromGpu,
    dataInOut,
    gpuCommandInt,
    gpuDataInt
);

// State machine and command buffer - only accept commands when 'idle',
// accept no commands during frame update, inject read/write layer header commands
// to update headers (animated, velocities) after a frame render event
commandBuffer inst_commandBuffer(
    gpuClk,
    rst,
    gpuBusyController,
    frameRendering,
    gpuCommandInt,
    gpuDataInt, 
    dataFromGpu,
    !readyBusy,
    gpuCommand,
    gpuData
);

endmodule