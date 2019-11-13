module pixelCountCtrl(
    input gpuClock, // 400 MHz clock
    input pixelFound_palette, // Pixel found in palette pipe?
    input [10:0] xPixel_pixelCnt, // X Pixel location in pixel count pipe
    input [10:0] yPixel_pixelCnt, // Y Pixel location in pixel count pipe
    input [10:0] xPixel_palette, // X Pixel location in palette pipe
    input [10:0] yPixel_palette, // Y Pixel location in palette pipe
    output reg pixelIncForce // Force pixel increment pulse
);

// Send a pixel increment pulse to pixel counter if the palette pipe found a 
// pixel on the same x-y coordinates that the pixel counter is on
always @(gpuClock) begin
    if (gpuClock && pixelFound_palette && 
        xPixel_pixelCnt == xPixel_palette && 
        yPixel_pixelCnt == yPixel_palette) 
        pixelIncForce <= 1;
    else 
        pixelIncForce <= 0;
end

endmodule