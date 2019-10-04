module ramAddressCalc(
    input unsigned [31:0] startAddress,
    input isSprite,
    input unsigned [7:0] frameNumber,
    input unsigned [15:0] width,
    input unsigned [15:0] xOffset,
    input unsigned [15:0] yOffset,
    input unsigned [15:0] characterIndex,
    output reg unsigned [31:0] endAddress //
);

// Data has 16 bit depth

always begin
    if (isSprite) begin
        endAddress <= startAddress +
            (frameNumber * height * width) + // Frame offset
            (yOffset * width) + xOffset; // Pixel X offset
    end
    else begin
        endaaddress <= startAddress +
            (characterIndex << 1); // 2 characters / 16 bits
    end
end

endmodule;