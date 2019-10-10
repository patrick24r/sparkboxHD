module ramAddressCalc(
    input isSprite,
    input unsigned [7:0] frameNumber,
	input unsigned [15:0] height,
    input unsigned [15:0] width,
    input signed [15:0] xOffset,
    input signed [15:0] yOffset,
    input unsigned [15:0] characterIndex,
    output reg unsigned [25:0] addressOffset //
);

always begin
    if (isSprite) begin
        addressOffset <= (frameNumber * height * width) + // Frame offset
            (yOffset * width) + xOffset; // Pixel X offset
    end
    else begin
        addressOffset <= (characterIndex << 1); // 2 characters / 16 bits
    end
end

endmodule