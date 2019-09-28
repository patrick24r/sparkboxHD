// This is the top level file for the sparkbox GPU
module sparkboxGPUtop(
    input cmd_clk_in,           // Clock for receiving GPU commands
    input cmd_inout,            // Command direction bit
    inout [7:0] command,        // Specifies the command for the GPU
    inout [15:0] commandData,   // Specifies the data for the given command
    output cmd_clk_out          // Clock for sending GPU read data
    // Include RAM / Flash external control signals here
    // Include HDMI outputs here
);

// HOW TO USE THIS GPU INTERFACE
// ----------------------------------------------------------------------------
// The GPU works by taking commands from an external source
//
// Commands only work when the device is not in the process of updating a frame
// 
// Once a frame is rendered, all commands sent during that frame execute
// before the next frame is rendered
//
// 

// Instance of the oscillator'/ main clock

// Instance of the controller generating control signals for the GPU
gpuControllerTop inst_controller(

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

// Instance of RAM/Flash pipe here

// Instance of 


endmodule