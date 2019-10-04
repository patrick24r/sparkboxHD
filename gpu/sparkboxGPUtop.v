`include "controllerTop.v"
`include "commandControlTop.v"
`include "layerHeadersTop.v"
`include "layerRamTop.v"

// This is the top level file for the sparkbox GPU
module sparkboxGPUtop(
    input cmd_clk_in,           // Clock for receiving GPU commands
    input cmd_inout,            // Command direction bit
    inout [15:0] command,        // Specifies the command for the GPU
    inout [15:0] commandData    // Specifies the data for the given command
    // Include RAM / Flash external control signals here
    // Include HDMI outputs here
);

// HOW TO USE THIS GPU INTERFACE
// ----------------------------------------------------------------------------
// The GPU works by taking commands from an external source
//
// All commands can be given at any time, but special behavior occurs during a frame render
// 
// Write commands are put on hold during a render event. Therefore, read commands sent during a render
// event will have priority over write commands sent during that same render. 
//
// For example, if before a render event, suppose the color stored in layer 3's palette color slot 5 is Red.
// Suppose a render event begins, then the following commands are received in this order:
// - Write to Palette Layer 3 Color 5 value (Color Gray)
// - Read from Palette Layer 3 Color 5
// The read command will execute first and return the color Red, while the write command will execute 
// immediately following the render event's completion
// 
// Once a frame is rendered, all commands sent during that frame execute
// before the next frame is rendered
//
// 

// Instance of the oscillator'/ main clock

// Instance of the controller generating control signals for the GPU
controllerTop inst_controller(

);

// Instance of the command control module
commandControlTop inst_commandBuffer(

);

// Instance of the pixel/layer counter stage 
// (includes palette control and HDMI output module)


// Instance of counter/header info pipe here

// Instance of layer header info module here
layerHeadersTop inst_layerheaderTop(

);

// Instance of header/RAM pipe here

// Instance of RAM module here
layerRamTop inst_layerRamTop(

);

// Instance of RAM/Flash pipe here

// Instance of 


endmodule