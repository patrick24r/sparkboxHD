// This module keeps track of the current pixel to be read from RAM
module pixelCounterTop
(
    input reset, // Reset counter (0 = reset, 1 = no reset)
    input pipelineClk, // Pipeline clock (layer increment pulse)
    input nextPixelPulse, // Current pixel done pulse (from controller)
    output [4:0] layer, // Up to 64 layers
    output [10:0] x, // 1920 (x) resolution
    output [10:0] y // 1080 (y) resolution
);

wire layerOverflowPulse; // Current pixel done pulse

// Instantiate all internal counters
layercnt layer_cnt_main(
    .layerInc(pipelineClk), 
    .reset(reset && !nextPixelPulse), // Reset layer on a new pixel pulse
    .overflow(layerOverflowPulse), 
    .layer(layer)
);

xcnt x_cnt_main(
    // Increment pixel on layer overflow or controller force update
    .pixelInc(nextPixelPulse || layerOverflowPulse), 
    .reset(reset), 
    .x(x)
);

ycnt y_cnt_main(
    // Increment pixel on layer overflow or controller force update
    .pixelInc(nextPixelPulse || layerOverflowPulse), 
    .x(x), 
    .reset(reset), 
    .y(y)
);

endmodule