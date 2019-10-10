module paletteMem(
    input clk,
    input rst,
    input writeEn,
    input [9:0] addr1,
    input [15:0] writeData,
    input [9:0] addr2,
    output [15:0] readData1,
    output [15:0] readData2
);

reg [15:0] paletteMemory [9:0];
integer i;

// Asynchronous reads as long as not under reset
assign readData1 = rst ? paletteMemory[addr1] : {16{1'b0}};
assign readData2 = rst ? paletteMemory[addr2] : {16{1'b0}};

initial begin
	for (i = 0; i < 1024; i = i + 1) begin
		 paletteMemory[i] = 16'd0;
   end
end


always @(posedge clk or negedge rst) begin
    if (!rst) begin
        // Reset all data to 0
        for (i = 0; i < 1024; i = i + 1) begin
            paletteMemory[i] <= 16'd0;
        end
    end
    else begin
        if (writeEn) begin
            paletteMemory[addr1] <= writeData;
        end
    end
end


endmodule