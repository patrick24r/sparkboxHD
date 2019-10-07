//`include "layercnt.v"
//`include "xcnt.v"
//`include "ycnt.v"

// This module keeps track of the current pixel to be read from RAM
// SUCCESS CONDITION:
// The inputs nextLayer and nextPixel MUST NEVER BE HIGH LONGER THAN 1 CLOCK CYCLE
module pixelCounterTop
(
    input clk, // Master clock
    input reset, // Reset counter (0 = reset, 1 = no reset)
    input nextLayer, // Curent layer done (1 = done, 0 = not done)
    input nextPixel, // Current pixel done (1 = done, 0 = not done)
    output [4:0] layer, // Up to 64 layers
    output [10:0] x, // 1920 (x) resolution
    output [10:0] y // 1080 (y) resolution
);

initial begin
    nextLayerPulse = 0;
    nextPixelPulse = 0;
end

reg nextPixelPulse;
reg nextLayerPulse;

// Instantiate all internal counters
layercnt layer_cnt_main(.layerInc(nextLayerPulse), .reset(reset), .layer(layer));
xcnt x_cnt_main(.pixelInc(nextPixelPulse), .reset(reset), .x(x));
ycnt y_cnt_main(.x(x), .reset(reset), .y(y));


// At positive edge of clk, determine to increment layer and x counters
// At negative edges, make sure pulses fall
always @(clk) begin
	 if (clk) begin
		if (nextPixel) begin
			// New pixel needed, also means new layer needed
         nextLayerPulse <= 1;
         nextPixelPulse <= 1;
		end else begin
         nextPixelPulse <= 0;
         if (nextLayer) nextLayerPulse <= 1;
         else nextLayerPulse <= 0;
		end
	 end else begin
		nextLayerPulse <= 0;
		nextPixelPulse <= 0;
	 end

end
endmodule