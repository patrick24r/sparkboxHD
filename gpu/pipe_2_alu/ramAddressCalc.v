module ramAddressCalc(
    input isSprite,
    input unsigned [7:0] frameNumber,
	input unsigned [15:0] height,
    input unsigned [15:0] width,
    input unsigned [15:0] xOffset,
    input unsigned [15:0] yOffset,
    input unsigned [15:0] characterIndex,
    output reg unsigned [25:0] addressOffset // In words, assuming 1 pixel = 16 bits = 1 word
);

always begin
    if (isSprite) begin
        addressOffset <= (frameNumber * height * width) + // Frame offset
            (yOffset * width) + xOffset; // Pixel X offset
    end
    else begin
        addressOffset <= (characterIndex >> 1); // 2 characters / 16 bits
    end
end

endmodule