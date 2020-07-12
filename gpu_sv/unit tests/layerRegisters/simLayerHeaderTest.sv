module simLayerHeaderTest(
	input clk, 
	input rst_n,
	input rst_layer_n,
	input unsigned [4:0] pipe_layer,
	input unsigned [4:0] ctrl_layer,
	input unsigned [2:0] ctrl_layer_register,
	input ctrl_write_en,
	input [15:0] ctrl_write_data,
	output logic [63:0] pipe_allRegisters,
	output logic [15:0] ctrl_read_data
);

logic [127:0] allReg;
assign pipe_allRegisters = allReg[63:0];

layerRegisters inst_test(
	clk, 
	rst_n,
	rst_layer_n,
	pipe_layer,
	ctrl_layer,
	ctrl_layer_register,
	ctrl_write_en,
	ctrl_write_data,
	allReg,
	ctrl_read_data
);

endmodule