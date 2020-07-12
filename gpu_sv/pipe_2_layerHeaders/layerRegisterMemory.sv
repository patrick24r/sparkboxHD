module layerRegisterMemory
#(
	parameter DATA_WIDTH = 16,
	parameter MEM_DEPTH = 32
)(
	input clk,
	input rst_n,
	input write_en,
	input [$clog2(MEM_DEPTH)-1:0] readWrite_addr,
	input [DATA_WIDTH-1:0] write_data,
	input [$clog2(MEM_DEPTH)-1:0] read_addr,
	output [DATA_WIDTH-1:0] readWrite_data,
	output [DATA_WIDTH-1:0] read_data
);

// This memory module features 
// * one port with r/w access
// * one port with r access
// * synchronous writes
// * asynchronous reads

logic [DATA_WIDTH-1:0] mem [MEM_DEPTH-1:0];
integer i;

// Initialize all data to 0
initial begin
    for (i = 0; i < MEM_DEPTH; i = i + 1) begin
        mem[i] = 'd0;
    end
end

// Asynchronous reads
assign readWrite_data = rst_n ? mem[readWrite_addr] : 'd0;
assign read_data = rst_n ? mem[read_addr] : 'd0;


always_ff @(posedge clk or negedge rst_n) begin
	if (!rst_n) begin
		// Reset all data to 0
		for (i = 0; i < MEM_DEPTH; i = i + 1) begin
			mem[i] <= 'd0;
		end
	end else begin
		i <= 0;
		if (write_en) begin
			mem[readWrite_addr] <= write_data;
		end
	end
end

endmodule