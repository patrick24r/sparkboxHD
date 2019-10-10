module flashAddressCalc(
    input unsigned [15:0] fontWidth,
    input unsigned [15:0] fontHeight,
    input signed [15:0] xOffset,
    input signed [15:0] yOffset,
    input unsigned [15:0] fontIndex,
    input unsigned [15:0] characterIndex,
    output unsigned [21:0] addressOffset
);

parameter charHpixels = 128, // character height in pixels
          charHbits = 7, // 2^7 = 128
          charWpixels = 64, // character width in pixels
          charWbits = 6, // 2^s6 = 64
          wordsPerCharacter = charHpixels * charWpixels / 16,
          charactersPerFont = 256,
          wordsPerFont = wordsPerCharacter * charactersPerFont;



assign addressOffset = 
    fontIndex * wordsPerFont + // Font offset
    characterIndex * wordsPerCharacter + // Character offset
    ((yOffset * fontHeight) >> (charHpixels + 4)) + // Y Position in character offset scaled
    (((xOffset % fontWidth) * fontWidth) >> (charWpixels + 4)); // X position in character offset scaled

endmodule