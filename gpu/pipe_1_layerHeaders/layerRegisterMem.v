// This layer serves as memory to store a register for all layers
// E.g. Stores register 0 for all layers
module layerRegisterMem(
    input clk,
    input reset, // master reset, active low
    input [4:0] readAddr1,
    input [4:0] readAddr2,
    input writeEn,
    input [4:0] writeAddr,
    input [15:0] writeData,
    output [15:0] readData1,
    output [15:0] readData2
);

// Creates an array of 32 registers 16 bits in size
reg [15:0] layerRegisters [31:0];
integer i;

// Initialize all data as blank
initial begin
    for (i = 0; i < 32; i = i + 1) begin
        layerRegisters[i] = 16'd0;
    end
end

// Asynchronous reads as long as not under reset
assign readData1 = reset ? layerRegisters[readAddr1] : {16{1'b0}};
assign readData2 = reset ? layerRegisters[readAddr2] : {16{1'b0}};

// Synchronous writes
// Writes on posedge
always @(posedge clk or negedge reset) begin
    if (!reset) begin
        // Reset all data to 0
        for (i = 0; i < 32; i = i + 1) begin
            layerRegisters[i] <= 16'd0;
        end
    end
    else begin
		  i <= 0;
        if (writeEn) begin
            layerRegisters[writeAddr] <= writeData;
        end
    end
end

endmodule