module commandBuffer(
    input pipelineClock,
    input reset,
    input gpuBusy, // 1 = GPU busy/rendering, 0 = GPU free/not rendering
    input [15:0] interfaceCmd, // Command received from interface
    input [15:0] interfaceData, // Data received from interface
    output reg [15:0] gpuCommand, // Command to send to GPU
    output reg [15:0] gpuData // Data to send to GPU
);

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
always @(posedge pipelineClock or negedge reset) begin
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
always @(negedge pipelineClock) begin
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