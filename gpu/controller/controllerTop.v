module controllerTop(
    input gpuClock, // 
    input reset, // Master reset

    input [15:0] command, // Command from I/O pins (buffered)
    input [15:0] data, // Data from I/O pins (buffered)

    // Pixel counter inputs
    input unsigned [10:0] pixelCntX, // Pixel in pixel counter pipe's x position
    input unsigned [10:0] pixelCntY, // Pixel in pixel counter pipe's y position

    // Layer header inputs
    input [15:0] layerHeaderData, // Data from layer headers

    // RAM inputs
    input ramDone, // RAM memory operations done
    input [15:0] ramData, // Data from RAM memory
    input unsigned [10:0] ramX, // Pixel in ram pipe's x position
    input unsigned [10:0] ramY, // Pixel in ram pipe's y position

    // Flash inputs
    input flashDone, // Flash memory operations done 
    input [15:0] flashData, // Data from flash memory
    input unsigned [10:0] flashX, // Pixel X position in flash pipe
    input unsigned [10:0] flashY, // Pixel in flash pipe's y position

    // Palette inputs
    input [15:0] paletteData,
    input pixelFound, // Non-transparent pixel found
    input unsigned [10:0] paletteX, // Pixel X position in palette pipe
    input unsigned [10:0] paletteY, // Pixel Y position in palette pipe


    // Pixel counter pipeline control bits
    output reg rstPixelInc, // Reset pixel counter
    output pixelInc, // Pixel increment pulse

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
    output controllerRGB, // Controller RGB select

    // Generic ouputs
    output [15:0] pipelineData, // Data to pipeline from external source
    output [4:0] commandLayer, // Layer specified by the command
    output reg [15:0] dataOut, // Data to external source
    output reg pipelineClock, // Pipeline clock
    output reg currentlyRendering, // 1 = Rendering, 0 = not rendering
    output gpuBusy // 1 = GPU busy, 0 = GPU not busy
);

parameter X_MAX = 1280,
			 Y_MAX = 720;

initial currentlyRendering = 0;
initial rstPixelInc = 1;


// All control signal(s) to Pipe 0, Pixel counter pipe
pixelCountCtrl inst_pixelCountCtrl(
    gpuClock, // Inputs
    pixelFound,
    pixelCntX,
    pixelCntY,
    paletteX,
    paletteY,

    pixelInc // Output
);

// All control signal(s) to Pipe 1, Layer headers pipe
layerHeaderCtrl inst_layerHeaderCtrl(
    command, // Input

    rstLayerheaders, // Outputs
    rstSingleLayer,
    wLayerHeaders
);

// All control signal(s) to Pipe 3, Layer RAM pipe
ramCtrl inst_ramCtrl(
    command[15:11], // Input

    rstRAM, // Outputs
    rRAM,
    wRAM
);

// All control signal(s) to Pipe 4, text Flash pipe
flashCtrl inst_flashCtrl(
    command[15:11], // Input

    rstFlash, // Outputs
    rFlash,
    wFlash
);

// All control signal(s) to Pipe 5, palette pipe
paletteCtrl inst_paletteCtrl(
    command, // Input

    rstPalette, // Outputs
    wPalette,
    controllerColor,
    controllerRGB
);


// Generic pipeline control outputs
// Data going to the pipeline is the data from I/O
assign pipelineData = data;
// Layer position specified by the command (0-31)
assign commandLayer = command[4:0];

// GPU is 'busy' if reading from flash/ram
assign gpuBusy = !(ramDone && flashDone);

// MUX data out based on command
always begin
    // MUX out data based on where command is reading/writing
    case (command[13:11])
    3'h0: dataOut <= 0; // All GPU memories (Not supported)
    3'h1: dataOut <= layerHeaderData; // Layer header data
    3'h2: dataOut <= ramData; // RAM (layer data)
    3'h3: dataOut <= paletteData; // Palette
    3'h4: dataOut <= flashData; // Flash (text glyph data)
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
 * In this example, the pipeline took 4 clock cycles to complete
 * The pipleine clock is also gated to only go high if a frame is rendering
 * 
 */
 always @(gpuClock or negedge reset) begin
    if (!reset) begin
        pipelineClock <= 0;
    end else begin
        if (gpuClock && currentlyRendering) begin
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
 end

// Determine if the GPU is currently rendering and whether or not to 
// reset the pixel counter
always @(gpuClock or negedge reset) begin
    if (!reset) begin
        currentlyRendering <= 0;
        rstPixelInc <= 0;
    end 
	 else begin
        // On negative edge, determine if palette found the last pixel
        if (!gpuClock) begin
            if (currentlyRendering) begin
                // Rendering and palette found last pixel, no longer rendering
                if (pixelFound && paletteX == (X_MAX - 1) && paletteY == (Y_MAX - 1)) currentlyRendering <= 1'b0; 
                // Palette did not find last pixel, still rendering
                else currentlyRendering <= 1'b1; 
                rstPixelInc <= 1;
            end else begin
                // Not rendering, begin rendering if update frame command is sent
                if (command == 16'h0001) begin
                    currentlyRendering <= 1'b1;
                    // Reset pixel counter
                    rstPixelInc <= 0;
                end else begin
                    currentlyRendering <= 1'b0;
                    rstPixelInc <= 1;
                end
            end
        end 
		  else begin
            currentlyRendering <= currentlyRendering;
            rstPixelInc <= 1;
        end
    end
end

endmodule