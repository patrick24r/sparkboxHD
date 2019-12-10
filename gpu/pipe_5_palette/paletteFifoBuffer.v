module paletteFifoBuffer(
    input clk_pipe, // Input data clock
    input clk_hdmi, // Output data clock
    input reset,
    input writeEn, // Enable write
    input [23:0] dataIn,
    output reg [23:0] dataOut,
    output [BUFFER_DEPTH-1:0] size,
    output empty, // 1 = buffer empty, 0 = buffer not empty
    output full // 1 = buffer full, 0 = buffer not full
);

parameter BUFFER_DEPTH = 8;

// Internal registers
reg [23:0] buffer [BUFFER_DEPTH-1:0];
reg [BUFFER_DEPTH-1:0] beginAddr;
reg [BUFFER_DEPTH-1:0] nextAddr;
integer i;


// "Full" if the next address would increment to become the begin address
assign full = nextAddr + 1 == beginAddr;
assign empty = nextAddr == beginAddr;
assign size = nextAddr - beginAddr;


initial begin
    for (i = 0; i < BUFFER_DEPTH; i=i+1) buffer[i] = 0;
    beginAddr = 0;
    nextAddr = 0;
end


// At positive edge of pipeline clock, clock in new data
// if writing is enabled and not under reset and won't overflow
always @(posedge clk_pipe or negedge reset) begin
    if (!reset) begin
        for (i = 0; i < BUFFER_DEPTH; i=i+1) buffer[i] <= 0;
        nextAddr <= 0;
    end else begin
        if (nextAddr + 1 == beginAddr || !writeEn) begin
            nextAddr <= nextAddr;
        end else begin
            buffer[nextAddr] <= dataIn;
            nextAddr <= nextAddr + 1'b1;
        end
        i <= 0;
    end
    
end

// At positive edge of hdmi clock, clock out data if
// the buffer is not empty
always @(posedge clk_hdmi or negedge reset) begin
    if (!reset) begin
        beginAddr <= 0;
        dataOut <= 0;
    end else begin 
        if (!empty) begin
            dataOut <= buffer[beginAddr];
            beginAddr <= beginAddr + 1'b1;
        end else begin
            beginAddr <= beginAddr;
            dataOut <= 0;
        end
    end
end


endmodule