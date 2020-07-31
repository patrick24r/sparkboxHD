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
    input [24:0] pipe_addr_bytes,

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

localparam STATE_IDLE = 0,
	STATE_PRE_OP = 1,
	STATE_CACHE_READ = 2,
	STATE_CACHE_WRITE = 3,
	STATE_SDRAM_WRITE = 4,
	STATE_SDRAM_READ = 5;

localparam CACHE_DEPTH = 32;
	

logic [2:0] state;
logic [23:0] final_addr_words;
logic [15:0] read_data_sdram, read_data_cache;
logic unsigned [6:0] sd_read_count;
logic sdram_read_en, sdram_write_en, sdram_bsy, sdram_read_rdy;
logic cache_write_en, cacheReadSuccess, cacheRst;

// Table of sdram start addresses for each layerID
logic [24:0] sdramAddressStart [7:0];
// For back-to-back controller writes/reads, keep track of the last place read/written
logic [7:0] ctrl_nextLayerID;
logic [23:0] ctrl_nextAddressOffset;

initial begin
	state = STATE_IDLE;
	sd_read_count = 0;
	for(int i = 0; i < $size(sdramAddressStart); i++) sdramAddressStart[i] = 0;
	ctrl_nextLayerID = 0;
	ctrl_nextAddressOffset = -1;
end


always_ff @(posedge clk_n or negedge rst) begin
	if (!rst) begin
		state <= STATE_IDLE;
		sd_read_count <= 0;
		for(int i = 0; i < $size(sdramAddressStart); i++) sdramAddressStart[i] <= 0;
		ctrl_nextLayerID <= 0;
		ctrl_nextAddressOffset <= -1;
	end else begin
		case (state)
			STATE_IDLE: begin
				// Wait for new read from the pipeline
				if (pipeline_clk && pipe_read_en) state <= STATE_CACHE_READ;
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
					
					// If writing to a new layer, save the new layer offset
					if (ctrl_nextLayerID != ctrl_layerId) begin
						if (ctrl_write_en) begin
							sdramAddressStart[ctrl_layerId] <= sdramAddressStart[ctrl_nextLayerID] + 
								ctrl_nextAddressOffset + 1;
						end
						ctrl_nextAddressOffset <= 0;
					end else ctrl_nextAddressOffset <= ctrl_nextAddressOffset + 1;
					
				end else begin
					ctrl_nextLayerID <= ctrl_nextLayerID;
					ctrl_nextAddressOffset <= ctrl_nextAddressOffset;
				end
				
			end
			
			STATE_CACHE_READ: begin
				if (cacheReadSuccess) state <= STATE_IDLE;
				else begin
					// Whatever we're reading isn't in cache, read from SDRAM
					state <= STATE_SDRAM_READ;
				end
			end
			
			STATE_SDRAM_READ: begin
				// Read directly from SDRAM
				// If it's a pipeline read, fill the cache
				// If it's a controller read, don't fill the cache
				if (sdram_read_rdy && (sd_read_count + 1 == CACHE_DEPTH || ctrl_read_en)) begin
					state <= STATE_IDLE;
					sd_read_count <= 0;
				end else if (sdram_read_rdy) begin
					state <= STATE_SDRAM_READ;
					sd_read_count <= sd_read_count + 1;
				end else begin
					state <= STATE_SDRAM_READ;
					sd_read_count <= sd_read_count;
				end
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
	// Controller reads bypass the cache
	read_data <= ctrl_read_en ? read_data_sdram : read_data_cache;
	
	// ----- Cache signals -----
	// Reset the cache when controller is reading/writing to SDRAM or on actual rst
	cacheRst <= (rst && !(state == STATE_SDRAM_WRITE || (state == STATE_SDRAM_READ && ctrl_read_en))); 
	// Only writing to the cache when reading sdram for the pipeline
	cache_write_en <= (state == STATE_SDRAM_READ && pipe_read_en && !ctrl_read_en);
	
	// ----- SDRAM signals -----
	sdram_read_en <= state == STATE_SDRAM_READ; 
	sdram_write_en <= state == STATE_SDRAM_WRITE;
	
	// Final read/write address in words
	final_addr_words <= (ctrl_read_en || ctrl_write_en) ? 
		sdramAddressStart[ctrl_nextLayerID] + ctrl_nextAddressOffset : // Controller final address (words)
		sdramAddressStart[pipe_layerId] + (pipe_addr_bytes >> 1); // Pipeline final address (words)
	
end


// SDRAM cache
layerRamCache #(.CACHE_DEPTH(CACHE_DEPTH))
inst_layerCache(
	.clk(!clk_n),
	.rst(cacheRst),
	.write_en(cache_write_en),
	.layer(pipe_layer),
	.addr_words(final_addr_words),
	.data_i(read_data_sdram), // Only ever write data from SDRAM
	.data_o(read_data_cache),
	.data_o_valid(cacheReadSuccess)
);



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
.rd_data(read_data_sdram), // read data
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