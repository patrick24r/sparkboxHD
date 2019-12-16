// This module interfaces with the external controller
module commandInterface(
    input outputEnable, // Output enable pin
    input commandClk, // Clock pin
    input [15:0] dataFromGpu, // Data from GPU for chip reads
    inout [15:0] dataInOut, // Data pins
    output reg [15:0] commandToGpu, // Command from interface to GPU
    output reg [15:0] dataToGpu // Data from interface to GPU
);
parameter STATE_CMD_NEXT = 1'b0,
			 STATE_DATA_NEXT = 1'b1;


reg cmd_state; // Accept ommand or data next
reg [15:0] cmdSave; // Save command

initial begin
    commandToGpu = 16'd0;
    dataToGpu = 16'd0;
	 cmd_state = STATE_CMD_NEXT;
	 cmdSave = 16'd0;
end


// Set input pins when output is enabled/disabled
assign dataInOut = outputEnable ? dataFromGpu : 16'bZ;


// Clock in command, then data to GPU on commandClk rising edge
// while output is disabled
always @(posedge commandClk) begin
    if (!outputEnable) begin
		  // Ouput disabled, clocking in command or data
        if (cmd_state == STATE_CMD_NEXT) begin
				// First clock cycle with output disabled 
				cmdSave <= dataInOut;
				dataToGpu <= 16'd0;
				commandToGpu <= 16'd0;
				cmd_state <= STATE_DATA_NEXT;
		  end else begin
				cmdSave <= 16'd0;
			   dataToGpu <= dataInOut;
				commandToGpu <= cmdSave;
				cmd_state <= STATE_CMD_NEXT;
		  end
    end else begin
        // Output is enabled, put read data from GPU on output
		  cmdSave <= 16'd0;
		  dataToGpu <= 16'd0;
        commandToGpu <= 16'd0;
		  cmd_state <= STATE_CMD_NEXT;
    end
end



endmodule