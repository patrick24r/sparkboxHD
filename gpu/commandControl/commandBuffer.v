module commandBuffer(
    input gpuClock,
    input reset,
    input gpuBusy, // 1 = GPU busy/rendering, 0 = GPU free/not rendering
    input [15:0] interfaceCmd, // Command received from interface
    input [15:0] interfaceData, // Data received from interface
    output reg [15:0] gpuCommand, // Command to send to GPU
    output reg [15:0] gpuData // Data to send to GPU
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
 * If targeting pallete, layer headers, or RAM memory
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
 * command[10:6] - Reserved
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



// Buffers for commands
reg [15:0] commandBuffer [3:0];
reg [15:0] dataBuffer [3:0];

// Offset of buffer for next valid command/data pair
unsigned reg [3:0] bufferOffset;
reg offsetFound;

// Initialize outputs and registers to all 0's
initial begin
    for (i = 0; i < 15; i = i + 1) begin
        commandBuffer[i] = 16'd0;
        dataBuffer[i] = 16'd0;
    end
    gpuCommand = 16'd0;
    gpuData = 16'd0;
    bufferOffset = 4'd0;
    offsetFound = 1'd0;
end

// Shift buffers
always @(posedge gpuClock or negedge reset) begin
    if (!reset) begin
        // Reset all internal registers
        for (i = 0; i < 15; i = i + 1) begin
            commandBuffer[i] <= 16'd0;
            dataBuffer[i] <= 16'd0;
        end
        gpuCommand <= 16'd0;
        gpuData <= 16'd0;
    end
    else begin
        // Always clock out the command at the current offset
        gpuCommand <= commandBuffer[bufferOffset];
        gpuData <= dataBuffer[bufferOffset];

        // Add next command to buffer
        // Note: if the command buffer is full, this will overwrite
        // the most recent command
        commandBuffer[15] <= interfaceCmd;
        dataBuffer[15] <= interfaceData;

        // Shift all data/command pairs until the offset is found
        // Offset is constrained to a value from 15-0
        for (i = 14; i >= bufferOffset; i = i - 1) begin
            commandBuffer[i] <= commandBuffer[i+1];
            dataBuffer[i] <= dataBuffer[i+1];
        end

    end

    // Reset the offsetFound flag
    offsetFound <= 1'b0;
end


// On the negative edge of the clock, determine the buffer offset for the next buffer shift
always @(negedge gpuClock) begin
    if (gpuBusy) begin
        // Stall GPU write/reset/special commands
        // Works by using a buffer offset that identifies the first
        // buffer position where a read command is present. 
        for (i = 0; i < 15 && !offsetFound; i = i + 1) begin
            // If the current command is a read command,
            // this command can be executed during render
            if (commandBuffer[i][15:14] == 2'b01) begin
                offsetFound <= 1'b1;
                bufferOffset <= i;
            end
        end
    end 
    else begin
        // Reset buffer offset variables
        bufferOffset <= 4'd0;
        offsetFound <= 1'b0;
    end
end

endmodule