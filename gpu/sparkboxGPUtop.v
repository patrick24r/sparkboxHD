// This is the top level file for the sparkbox GPU
module sparkboxGPUtop(
	 input clk_oscillator,

	 // Command interface pins
    input cmd_clk,              // Clock for receiving GPU commands
    input cmd_outputEnable,     // Output enable pin
    inout [15:0] cmd_data,      // GPU command and data lines
	 output cmd_readyBusy,       // RDY/#BSY O pin, 1 = ready, 0 = busy
	 
    // SDRAM control signals
	 
    // Flash control signals
	 
    // HDMI transmitter signals here
	 output [23:0] pixelBus 
);

/* HOW TO USE THIS GPU INTERFACE
 * ----------------------------------------------------------------------------
 * The GPU works by taking commands from an external source
 *
 * All commands can be given at any time, but commands are ignored during a render 
 * event or when a previous command is busy executing The ready/#busy pin indicates
 * if the device is busy (1 = ready, 0 = busy).
 * 
 * SENDING A COMMAND
 * ----------------------------------------------------------------------------
 * When the device is not busy, disable output by pulling the output enable pin low.
 * 
 * 
 */ 

 
/* INTERNAL WIRES BEGIN */
 
wire
gpu_clock, // 400 MHz
pipeline_clock,
 
// Command control outputs
[15:0] o_cmdCtrl_command, // Output from command control, command
[15:0] o_cmdCtrl_data,

// Controller outputs
o_controller_gpuBusy,
o_controller_frameRendering,
o_controller_data,
 
o_controller_rstPixelCounter;
 
 
 /* INTERNAL WIRES END */
 
 
// List 8 debug LEDs here

// List switches and buttons here


// Instance of the oscillator / main clock module
// to turn one clock frequency into 400MHz
pll inst_pll(
    .inclk0(CLK_50MHZ), // input clock (may not be 50 MHz)
    .c0(gpu_clock) // output clock of 400 MHz
);

// Instance of the command control module
commandControlTop inst_commandControl(
	 .gpuClk(gpu_clock),
    .rst(KEY[0]),
    .gpuBusyController(o_controller_gpuBusy),
    .frameRendering(o_controller_frameRendering), // Controller frame rendering signal
    .outputEnable(cmd_outputEnable),
    .commandClk(cmd_clk),
    .dataFromGpu(o_controller_data),
    .dataInOut(cmd_data),
    .readyBusy(cmd_readyBusy), 
    .gpuCommand(o_cmdCtrl_command), // Command to GPU controller
    .gpuData(o_cmdCtrl_data) // Data to GPU controller
);

// Instance of the controller generating control signals for the GPU
controllerTop inst_controller(
	 .gpuClock(gpu_clock), // 
    .reset(KEY[0]), // Master reset

    .command(o_cmdCtrl_command), // Command from I/O pins (buffered)
    .data(o_cmdCtrl_data), // Data from I/O pins (buffered)

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
    rstPixelInc(o_controller_rstPixelCounter), // Reset pixel counter
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
    .dataOut(o_controller_data), // Data to external source
    .pipelineClock(pipeline_clock), // Pipeline clock
    .currentlyRendering(o_controller_frameRendering), // 1 = Rendering, 0 = not rendering
    .gpuBusy(o_controller_gpuBusy) // 1 = GPU busy, 0 = GPU not busy
);

/* ---------------------------- BEGIN PIPELINE ---------------------------- */
// Instance of the pixel/layer counter stage 
pixelCounterTop inst_pixelCounterTop(

);

// Instance of counter/header info pipe here

// Instance of layer header info module here
layerHeadersTop inst_layerheaderTop(

);

// Instance of header/ALU pipe here

// Instance of ALU module here
aluTop inst_aluTop(

);

// Instance of ALU/RAM pipe here

// Instance of RAM module here
layerRamTop inst_layerRamTop(

);

// Instance of RAM/Flash pipe here

// Instance of Flash module here
textFlashTop inst_testFlashTop(

);

// Instance of Flash/palette pipe here

// Instance of palette module here
paletteTop inst_paletteTop(

);

// No pipeline register here, palette fifo buffer acts as pipeline register

// Instance of pixel output module here

/* ---------------------------- END PIPELINE ---------------------------- */

// Instance of HDMI transmitter signals here

endmodule