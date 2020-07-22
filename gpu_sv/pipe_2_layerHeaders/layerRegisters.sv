module layerRegisters(
	input pipeline_clk_n, // Input clk (pipeline clock negated)
	input rst_n, // Master reset switch
	input rst_layer_n, // Resets a layer specified by ctrl_layer
	input unsigned [4:0] pipe_layer, // Layer the pipeline wants to read
	input unsigned [4:0] ctrl_layer, // Layer the controller wants to read/write
	input unsigned [2:0] ctrl_layer_register, // Register the controller wants to read/write
	input ctrl_write_en, // Write enable line for controller
	input [15:0] ctrl_write_data, // Data for the controller to write
	output logic [127:0] pipe_allRegisters, // All registers for a layer - meant for the pipeline
	output logic [15:0] ctrl_read_data // Data read from the registers for the controller
);

logic [127:0] ctrl_allRegisters;


// Mux out the register the controller needs
always_comb begin
	case (ctrl_layer_register)
		0: ctrl_read_data <= ctrl_allRegisters[15:0];
		1: ctrl_read_data <= ctrl_allRegisters[31:16];
		2: ctrl_read_data <= ctrl_allRegisters[47:32];
		3: ctrl_read_data <= ctrl_allRegisters[63:48];
		4: ctrl_read_data <= ctrl_allRegisters[79:64];
		5: ctrl_read_data <= ctrl_allRegisters[95:80];
		6: ctrl_read_data <= ctrl_allRegisters[111:96];
		7: ctrl_read_data <= ctrl_allRegisters[127:112];
	endcase
end

// This also supports resetting an entire layer when the controller wants to
genvar i;
generate
	// Generate a memory module for each register
	for (i=0; i < 8; i=i+1) begin : generate_layer_memory
		layerRegisterMemory inst_layerMemory(
			.clk(pipeline_clk_n),
			.rst_n(rst_n),
			
			// Controller signals
			// Only write to the specified layer register
			.write_en(rst_layer_n ? (ctrl_write_en && ctrl_layer_register == i) : 1), 
			.readWrite_addr(ctrl_layer), // Layer from controller is the address
			.write_data(rst_layer_n ? ctrl_write_data : 'd0), // Data to write from controller
			.readWrite_data(ctrl_allRegisters[15+i*16:i*16]), // Data read from the controller r/w port
			
			// Pipeline signals
			.read_addr(pipe_layer),
			.read_data(pipe_allRegisters[15+i*16:i*16])
		);
	end
endgenerate


endmodule