module layerRamTop(
    input clk, // 50 MHz clock
    input pipeline_clk,
    input rst,
    output rdy, // 1 = ready, 0 = busy
    output [15:0] read_data,

    // Controller specific interface
    input ctrl_read_en,
    input ctrl_write_en,
    input [24:0] ctrl_addr_bytes,
    input [15:0] ctrl_write_data,

    // Pipeline specific interface
    input pipe_read_en,
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

logic [24:0] read_addr_bytes;
logic [15:0] data_read_raw;
logic read_en, write_en, read_rdy;

// If the byte address specifies the upper word, only give the upper word
assign data_read = read_addr_bytes[0] ? {8'd0, data_read_raw[15:8]} : data_read_raw;

// Operation priorites: 
// 0: controller write
// 1: controller read
// 2: pipeline read
always_ff @(posedge clk or negedge rst) begin
    if (!rst) begin
        read_addr_bytes <= 0;
        read_en <= 0;
        write_en <= 0;
    end else begin
        if (pipeline_clk) begin
            // Prioritize controller reads over pipeline reads
            // The controller uses 16 bit reads but we may need 8 bit
            if (ctrl_read_en) read_addr_bytes <= ctrl_addr_bytes;
            else read_addr_bytes <= pipe_addr_bytes;

            // Prioritize controller writes over reads
            read_en <= !ctrl_write_en && (pipe_read_en || ctrl_read_en);
            write_en <= ctrl_write_en;
        end else begin
            read_addr_bytes <= 0;
            read_en <= 0;
            write_en <= 0;
        end
    end
end

// SDRAM controller
assign sdram_clk = clk;
sdram_controller #(.CLK_FREQUENCY(50), .ROW_WIDTH(13), .COL_WIDTH(9), .BANK_WIDTH(2)) inst_sdram(
.clk(clk),
.rst_n(rst),

// Host interface
.wr_addr(ctrl_addr), // write address
.wr_data(ctrl_write_data), // write data
.wr_enable(write_en), // write enable
.rd_addr(read_addr_bytes[24:1]), // read address, convert byte address to word address
.rd_data(data_read_raw), // read data
.rd_ready(read_rdy), // read data is ready
.rd_enable(read_en), // read enable
.busy(!rdy), // busy flag

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