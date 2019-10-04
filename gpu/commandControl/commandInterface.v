// This module interfaces with the external controller
module commandInterface(
    input chipSelect, // Chip select pin
    input outputEnable, // Output enable pin
    input commandClk, // Clock pin
    input [15:0] inputCommand, // Command pins
    input [15:0] dataFromGpu, // Data from GPU for chip reads
    inout reg [15:0] dataInOut, // Data pins
    output reg [15:0] commandToGpu, // Command from interface to GPU
    output reg [15:0] dataToGpu // Data from interface to GPU
);

initial begin
    dataInOut = 16'bZ;
    commandToGpu = 16'd0;
    dataToGpu = 16'd0;
end

// Clock in commands to GPU on commandClk rising edge
always @(posedge commandClk) begin
    // Make sure chip is selected and output
    // is currently DISABLED (Chip is in write mode)
    if (chipSelect && !outputEnable) begin
        commandToGpu <= inputCommand;
        dataToGpu <= dataInOut;
    end
    else begin
        // Chip not selected or GPU output is ENABLED
        // (Chip is in read mode)
        dataToGpu <= 16'd0;
        commandToGpu <= 16'd0;
    end
end

// Clock out data for reads on commandClk negative edge
always @(negedge commandClk) begin
    // If output is enabled, put read data from GPU on output
    if (outputEnable && chipSelect) dataInOut <= dataFromGpu;
    else dataInOut <= 16'bZ;
end

endmodule