module flashAddressCalc(
    input unsigned [15:0] fontWidth,
    input unsigned [15:0] fontHeight,
    input unsigned [15:0] xOffset,
    input unsigned [15:0] yOffset,
    input unsigned [15:0] fontIndex,
    output unsigned [29:0] addressOffsetBits
);

parameter charHpixels = 128, // character height in pixels
          charHbits = 7, // 2^7 = 128
          charWpixels = 64, // character width in pixels
          charWbits = 6, // 2^6 = 64
          bitsPerCharacter = charHbits + charWbits,
          charactersPerFont = 256, // Characters in each font
          bitCharactersPerFont = 8, // 2^8 = 256
          bitsPerFont = bitsPerCharacter + bitCharactersPerFont;


// NOTE: THIS CALCULATION DOES NOT INCLUDE CHARACTER OFFSET. CHARACTER OFFSET CANNOT BE ADDED
// UNITL THE CHARACTER IS READ FROM RAM
// Get address offset in bits (Bitmap in  flash for 
assign addressOffsetBits = 
    fontIndex << bitsPerFont + // Font offset
    (((yOffset * (charHpixels-1)) / (fontHeight-1)) << charWbits) + // Y Position in character offset scaled
    (((xOffset % fontWidth) * (charWpixels-1)) / (fontWidth-1)); // X position in character offset scaled

endmodule