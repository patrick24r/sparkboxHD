module paletteMem
#(
  parameter NUMBER_OF_LAYERS = 32,
  parameter PALETTE_SIZE = 32
)(
  input clk_n,
  input rst,

  // Controller inputs
  input ctrl_write_en,
  input ctrl_read_en,
  input unsigned [LAYER_WIDTH-1:0] ctrl_layer,
  input unsigned [PALETTE_WIDTH-1:0] ctrl_palette_idx,
  input [23:0] ctrl_data_i,

  // Pipeline inputs
  input unsigned [LAYER_WIDTH-1:0] pipe_layer,
  input unsigned [PALETTE_WIDTH-1:0] pipe_palette_idx,

  // Outputs
  output [23:0] data_o,
  output pixel_valid
);

localparam LAYER_WIDTH = $clog2(NUMBER_OF_LAYERS);
localparam PALETTE_WIDTH = $clog2(PALETTE_SIZE);

logic [23:0] paletteMem[LAYER_WIDTH-1:0][PALETTE_WIDTH-1:0];

initial begin
	for (int i = 0; i < NUMBER_OF_LAYERS; i++)
		for (int j = 0; j < PALETTE_SIZE; j++)
			paletteMem[i][j] = 0;
end

// Synchronous writes on positive edge of clock
always_ff @(posedge clk_n or negedge rst) begin
	if (!rst) begin
		for (int i = 0; i < NUMBER_OF_LAYERS; i++)
			for (int j = 0; j < PALETTE_SIZE; j++)
				paletteMem[i][j] <= 0;
	end else begin
		if (ctrl_write_en) 
			paletteMem[ctrl_layer][ctrl_palette_idx] <= ctrl_data_i;
	end
end


// Asynchronous reads
always_comb begin
  data_o <= ctrl_read_en ? 
	paletteMem[ctrl_layer][ctrl_palette_idx] : 
	paletteMem[pipe_layer][pipe_palette_idx];
  pixel_valid <= (data_o != 0);
end

endmodule
