module layerRamCache
#(
	parameter ADDR_WIDTH_WORDS = 24,
	parameter CACHE_DEPTH = 32,
	parameter MAX_LAYERS = 32
)(
	input clk, // 50 MHz clock (not inverted)
	input rst,
	input write_en,
	input unsigned [MAX_LAYERS_WIDTH-1:0] layer,
	input unsigned [ADDR_WIDTH_WORDS-1:0] addr_words,
	input unsigned [15:0] data_i,
	
	// Debug inputs
	input unsigned [MAX_LAYERS_WIDTH-1:0] addrStartLayer,

	// Cache outputs
	output logic [15:0] data_o,
	output logic data_o_valid,
	
	// Debug outputs
	output [ADDR_WIDTH_WORDS-1:0] cacheAddressOffset,
	output testOut,
	output [ADDR_WIDTH_WORDS-1:0] addrStarts
);

// This is the cache block for the layer RAM
// It is meant to hold cached read data from reading SDRAM for each layer
// Default parameters: 2 bytes * 32 layers * 32 cache slots per layer = 2 kB cache

localparam CACHE_WIDTH = $clog2(CACHE_DEPTH);
localparam MAX_LAYERS_WIDTH = $clog2(MAX_LAYERS);

// Table of what start addresses are currently in cache
logic unsigned [ADDR_WIDTH_WORDS-1:0] cacheAddressStart [MAX_LAYERS_WIDTH-1:0];
logic [15:0] cache [CACHE_WIDTH+MAX_LAYERS_WIDTH-1:0];
// Calculation of address offset
// logic unsigned [ADDR_WIDTH_WORDS-1:0] cacheAddressOffset;

assign addrStarts = cacheAddressStart[addrStartLayer];

initial begin
	for(int i = 0; i<$size(cacheAddressStart); i++) cacheAddressStart[i] = 0;
end

// Synchronous writes on positive edge
always_ff @(posedge clk or negedge rst) begin
	if(!rst) begin
		// Clear cache and all internal registers
		for(int i=0; i<$size(cacheAddressStart); i++) cacheAddressStart[i] <= 0;
	end else begin
		if (write_en && cacheAddressOffset >= CACHE_DEPTH) begin
			cacheAddressStart[layer] <= addr_words;
		end
		
		if (write_en) cache[{layer, cacheAddressOffset[CACHE_WIDTH-1:0]}] <= data_i;
	end
end


// Asynchronous reads
// This assumes that the cache is always filled. This means the caller must
// refill the layer they are replacing
always_comb begin
	data_o <= cache[{layer, cacheAddressOffset[CACHE_WIDTH-1:0]}];
	data_o_valid <= (write_en || cacheAddressOffset < CACHE_DEPTH);
end

assign cacheAddressOffset = $unsigned(addr_words) - $unsigned(cacheAddressStart[layer]);
assign testOut = cacheAddressOffset < CACHE_DEPTH;
endmodule