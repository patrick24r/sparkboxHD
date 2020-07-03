module fpgaRamTester(
	input CLOCK_50, // 50 MHz clock
	
	input [1:0] KEY, // On board push buttons
	output [7:0] LED, // On board LEDs
	
	// SDRAM interface
	output [12:0] DRAM_ADDR, // address
	inout [15:0] DRAM_DQ, // data
	output [1:0] DRAM_BA, // Bank address
	output [1:0] DRAM_DQM, // Data mask
	output DRAM_RAS_N,
	output DRAM_CAS_N,
	output DRAM_CKE,
	output DRAM_CLK,
	output DRAM_WE_N,
	output DRAM_CS_N
	
);

// Define states
// States
typedef enum {
	STATE_INIT = 0,
	STATE_WRITE = 1,
	STATE_READ = 2,
	STATE_CONFIRM = 3
} test_states;
	
test_states state;

// Define registers used for sdram controller interface

logic read_en, read_rdy, write_en, busy;
logic [23:0] addr;
logic [15:0] data_write;
logic [15:0] data_read;

logic [15:0] last_data_read;

// Set the LEDs to show the state
assign LED[5:1] = state;
assign LED[6] = KEY[0];
assign LED[7] = KEY[1];

// Initialize the state
initial begin
	state = STATE_INIT;
	LED[0] = 0;
end

// SDRAM test state machine
always_ff @(posedge CLOCK_50 or negedge KEY[1]) begin
	if (!KEY[1]) begin
		// Reset
		state <= STATE_INIT;
	end else begin
		case(state)
			// init - wait for button 0 to be pressed
			STATE_INIT: begin
				LED[0] <= LED[0];
				if (KEY[0])
					state <= STATE_INIT;
				else
					state <= STATE_WRITE;
			end

			// write - perform write operation
			STATE_WRITE: begin
				LED[0] <= LED[0];
				// Wait until not busy
				if (!busy)
					state <= STATE_READ;
				else
					state <= STATE_WRITE;
			end

			// read - perform read operation
			STATE_READ: begin
				LED[0] <= LED[0];
				// Wait until read data is ready
				if (read_rdy) begin
					state <= STATE_CONFIRM;
					// Save the read data
					last_data_read <= data_read;
				end else
					state <= STATE_READ;
			end

			// confirm - See if read/write data is the same, indicate on the LEDs
			STATE_CONFIRM: begin
				// Light up LED0 if read and write was successful
				LED[0] <= (last_data_read == data_write);
				// Wait for button to be released to go back to init
				if (KEY[0])
					state <= STATE_INIT;
				else
					state <= STATE_CONFIRM;
			end
			
			default: begin
				state <= STATE_INIT;
			end
		endcase
	end
end

// Generate sdram controller signals based on state
always_comb begin
	write_en <= state == STATE_WRITE;
	read_en <= state == STATE_READ;
	addr <= 24'h5;
	data_write <= 16'hdeaf;
end


assign DRAM_CLK = CLOCK_50;
// SDRAM controller
sdram_controller #(.CLK_FREQUENCY(50)) inst_sdram(
.clk(CLOCK_50),
.rst_n(KEY[1]), // Button 1 is the reset button

// Host interface
.wr_addr(addr), // write address
.wr_data(data_write), // write data
.wr_enable(write_en), // write enable

.rd_addr(addr), // read address
.rd_data(data_read), // read data
.rd_ready(read_rdy), // read data is ready
.rd_enable(read_en), // read enable

.busy(busy), // busy flag

// SDRAM interface
.clock_enable(DRAM_CKE),
.cs_n(DRAM_CS_N),
.ras_n(DRAM_RAS_N),
.cas_n(DRAM_CAS_N),
.we_n(DRAM_WE_N),
.data_mask_low(DRAM_DQM[1]),
.data_mask_high(DRAM_DQM[0]),
.addr(DRAM_ADDR),
.bank_addr(DRAM_BA),
.data(DRAM_DQ)
);


endmodule