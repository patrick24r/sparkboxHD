module layerRamTop(
    input clk_n, // 50 MHz clock negated
    input pipeline_clk,
    input rst,
    output rdy, // 1 = ready, 0 = busy
    output [15:0] read_data,

    // Controller specific interface
    input ctrl_read_en,
    input ctrl_write_en,
	 input [7:0] ctrl_layerId,
    input [15:0] ctrl_write_data,

    // Pipeline specific interface
    input pipe_read_en,
	 input [5:0] pipe_layer,
	 input [7:0] pipe_layerId,
    input [23:0] pipe_addr_bytes,

    // SDRAM interface
	output [12:0] sdram_addr, // address
	inout [15:0] sdram_dq, // data
	output [1:0] sdram_ba, // Bank address
	output [1:0] sdram_dqm, // Data mask
	output sdram_ras_n,
	output sdram_cas_n,
	output sdram_cke,
	output sdram_clk,
	output sdram_we_n,
	output sdram_cs_n
);

logic [1:0] state;

localparam STATE_IDLE = 0,
	STATE_SDRAM_WRITE = 1,
	STATE_SDRAM_READ = 2;


logic [23:0] final_addr_words;
logic [15:0] read_data_sdram;
logic sdram_read_en, sdram_write_en, sdram_bsy, sdram_read_rdy;

// Table of sdram start addresses for each layer
logic unsigned [23:0] sdramAddressStart [7:0];
// For back-to-back controller writes/reads, keep track of the last place read/written
logic unsigned [7:0] ctrl_nextLayerID;
logic unsigned [23:0] ctrl_nextAddressOffset;

always_ff @(posedge clk_n or negedge rst) begin
	if (!rst) begin
		for(int i = 0; i < $size(sdramAddressStart); i++) sdramAddressStart[i] <= 0;
		state <= STATE_IDLE;
		ctrl_nextLayerID <= 0;
		ctrl_nextAddressOffset <= 0;
	end else begin
		case (state)
			STATE_IDLE: begin
				// Wait for new address from the pipeline
				if (pipeline_clk) state <= STATE_SDRAM_READ;
				// Or until the controller tries to read/write
				else if (ctrl_write_en)
					// Writing to SDRAM, support consecutive writes
					state <= STATE_SDRAM_WRITE;
				else if (ctrl_read_en)
					// Reading SDRAM directly, support consecutive reads
					state <= STATE_SDRAM_READ;
				else state <= STATE_IDLE;
				
				// If reading/writing to SDRAM, support consecutive reads/writes
				if (ctrl_read_en || ctrl_write_en) begin
					ctrl_nextLayerID <= ctrl_layerId;
					
					if (ctrl_nextLayerID != ctrl_layerId) begin
						if (ctrl_write_en) sdramAddressStart[ctrl_layerId] <= ctrl_nextAddressOffset;
						ctrl_nextAddressOffset <= 0;
					end else ctrl_nextAddressOffset <= ctrl_nextAddressOffset + 1;
					
				end else begin
					ctrl_nextLayerID <= ctrl_nextLayerID;
					ctrl_nextAddressOffset <= ctrl_nextAddressOffset;
				end
				
			end
			
			STATE_SDRAM_READ: begin
				// Read directly from SDRAM
				if (sdram_read_rdy) state <= STATE_IDLE;
				else state <= STATE_SDRAM_READ;
			end
			
			STATE_SDRAM_WRITE: begin
				// Write to SDRAM and empty the cache
				if (!sdram_bsy) begin
					state <= STATE_IDLE;
				end else begin
					state <= STATE_SDRAM_WRITE;
				end
				
			end
			
		endcase
	end
end


always_comb begin
	// ----- Module signals -----
	// Only ready in the idle state
	rdy <= (state == STATE_IDLE);
	
	// ----- SDRAM signals -----
	sdram_read_en <= state == STATE_SDRAM_READ; 
	sdram_write_en <= state == STATE_SDRAM_WRITE;
	
	// Final read/write address in words
	final_addr_words <= (ctrl_read_en || ctrl_write_en) ? 
		sdramAddressStart[ctrl_nextLayerID] + ctrl_nextAddressOffset : // Controller final address (words)
		sdramAddressStart[pipe_layerId] + (pipe_addr_bytes >> 1); // Pipeline final address (words)
	
end

// SDRAM controller
assign sdram_clk = clk_n;
sdram_controller #(.CLK_FREQUENCY(50), .ROW_WIDTH(13), .COL_WIDTH(9), .BANK_WIDTH(2)) inst_sdram(
.clk(clk_n),
.rst_n(rst),

// Host interface
.wr_addr(final_addr_words), // write address
.wr_data(ctrl_write_data), // write data
.wr_enable(sdram_write_en), // write enable
.rd_addr(final_addr_words), // read address, convert byte address to word address
.rd_data(read_data), // read data
.rd_ready(sdram_read_rdy), // read data is ready
.rd_enable(sdram_read_en), // read enable
.busy(sdram_bsy), // busy flag

// SDRAM interface
.clock_enable(sdram_cke),
.cs_n(sdram_cs_n),
.ras_n(sdram_ras_n),
.cas_n(sdram_cas_n),
.we_n(sdram_we_n),
.data_mask_low(sdram_dqm[1]),
.data_mask_high(sdram_dqm[0]),
.addr(sdram_addr),
.bank_addr(sdram_ba),
.data(sdram_dq)
);


endmodule