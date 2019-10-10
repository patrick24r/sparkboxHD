module controllerTop(
    input gpuClock, // 
    input reset, // Master reset

    input [15:0] command, // Command from I/O pins (buffered)
    input [15:0] data, // Data from I/O pins (buffered)

    // Pixel counter inputs
    input [10:0] pixelCntX, // Pixel in pixel counter pipe's x position
    input [10:0] pixelCntY, // Pixel in pixel counter pipe's y position

    // RAM inputs
    input ramDone, // RAM memory operations done
    input [15:0] ramData, // Data from RAM memory
    input [10:0] ramX, // Pixel in ram pipe's x position
    input [10:0] ramY, // Pixel in ram pipe's y position

    // Flash inputs
    input flashDone, // Flash memory operations done 
    input [15:0] flashData, // Data from flash memory
    input [10:0] flashX, // Pixel X position in flash pipe
    input [10:0] flashY, // Pixel in flash pipe's y position

    // Palette inputs
    input [15:0] paletteData,
    input pixelFound, // Non-transparent pixel found
    input [10:0] paletteX, // Pixel X position in palette pipe
    input [10:0] paletteY, // Pixel Y position in palette pipe


    // Pixel counter pipeline control bits
    output reg pixelInc, // Pixel increment pulse

    // Layer header pipe control bits
    output rstLayerheaders, // Reset all layer headers
    output rstSingleLayer, // Reset 1 layer header
    output [2:0] layerRegister, // Index of layer header register for r/w
    output wLayerHeaders, // write layer headers enable bit

    // RAM pipe control bits
    output rstRAM, // Reset RAM
    output rRAM, // read RAM enable bit
    output wRAM, // write RAM enable bit

    // Flash pipe control bits
    output rstFlash, // Reset Flash
    output rFlash, // read Flash enable bit
    output wFlash, // write Flash enable bit

    // Palette pipe control bits
    output rstPalette, // Reset palette
    output wPalette, // Write palette enable
    output [4:0] controllerColor, // Selects palette color slot
    output [1:0] controllerRGB, // Controller RGB select

    // Generic ouputs
    output [15:0] pipelineData, // Data to pipeline from external source
    output [4:0] commandLayer, // Layer specified by the command
    output reg [15:0] dataOut, // Data to external source
    output reg pipelineClock // Pipeline clock
);



// Pixel counter control outputs
// Force increment pixel if still on that pixel
always @(gpuClock) begin
    if (pixelFound && gpuClock && pixelCntX == paletteX && pixelCntY == paletteY) pixelInc <= 1;
    else pixelInc <= 0;
end

// Layer header control outputs
assign rstLayerheaders = !(command[15:11] == 0 || (command[15:11] == 5'b00001 && command[5])); // Reset all layers
assign rstSingleLayer = !(command[15:11] == 5'b00001 == 0 && !command[5]); // Reset 1 layer
assign layerRegister = command[8:6]; // Layer register select
assign wLayerHeaders = (command[15:11] == 5'b10001); // write layer headers

// RAM control outputs
assign rstRAM = !(command[15:11] == 0 || command[15:11] == 5'b00010); // Reset RAM
assign rRAM = (command[15:11] == 5'b01010); // read RAM enable
assign wRAM = (command[15:11] == 5'b10010); // Write RAM enable

// Flash control outputs
// Must specifically target flash to reset it to protect the glyph data
assign rstFlash = !(command[15:11] == 5'b00100); 
assign rFlash = (command[15:11] == 5'b01100); // read Flash enable
assign wFlash = (command[15:11] == 5'b10100); // Write Flash enanble

// Palette control outputs
assign wPalette = (command[15:11] == 5'b10011); // write palette enable


// Generic pipeline control outputs
// Data going to the pipeline is the data from I/O
assign pipelineData = data;
// Layer position specified by the command (0-31)
assign commandLayer = command[4:0];

// MUX data out based on command
always begin
    // MUX out data based on where command is reading/writing
    case (command[13:11])
    3'h0: dataOut <= 0; // All GPU memories (Not supported)
    3'h1: dataOut <= ramData; // RAM (layer data)
    3'h2: dataOut <= paletteData; // Palette
    3'h3: dataOut <= flashData; // Flash (text glyph data)
    default: dataOut <= 0;
    endcase
end

/* Make sure that the pipeline clock is derived from the gpuClock (synchronous rising edges)
 * but only with certain clock pulses gated/bubbled, resulting in "missing" pulses
 * 
 * Example: 
 *                    _   _   _   _   _
 * gpuClock         _| |_| |_| |_| |_| |_
 *                    _               _
 * pipeline clock   _| |_____________| |_
 *
 * In this example, the pipeline took 4 clock cycles to oomplete
 *
 * TODO: If a pixel is found for a given x&y position on a higher layer, 
 * advance pipeline if flash and ram also are looking for that x&y position
 * regardless of whether they are done or not
 * 
 */
 always @(gpuClock) begin
    if (gpuClock) begin
        // If a pixel is found for an xy combo in flash and ram pipes,
        // that operation does not need to finish, the data will be unused
        if (pixelFound && 
            paletteX == flashX &&
            paletteY == flashY &&
            paletteX == ramX &&
            paletteY == ramY)  pipelineClock <= 1;
        else begin
            // Flash and RAM operations complete advance pipeline
            if (flashDone && ramDone) pipelineClock <= 1;
            else pipelineClock <= 0;
        end
    end
    else pipelineClock <= 0;
 end

endmodule