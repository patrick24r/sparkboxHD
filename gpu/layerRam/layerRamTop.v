module layerRamTop(
    input gpuClock, // Internal GPU clock
    input reset, // Internal reset 
    input pipelineClock, // Internal pipeline clock

    input controllerReadEn, // Determines if controller is reading data
    input controllerWriteEn, // Determines if controller is writing data
    input controller

    output reg doneRam // Done accessing RAM, pipeline can advance
);

// 2.5 ns gpuClock period (400 MHz)
// 15 ns write time (6 clock cycles)
// 700 ps read time (1 clock cycle)

// Determine if currently reading for controller or pipeline
// Controller reads/writes have priority over pipeline reads
parameter   state_reset = 2'b00, // "Reset" state, pipeline just
            state_readController = 2'b01, // Reading from RAM to controller
            state_readPipeline = 2'b10, // Reading from RAM to pipeline
            state_write = 2'b11; // Writing to RAM (data from controller)
reg [1:0] state;


// Keep track of the clock cycles
reg [4:0] clockCounter;
// Enable the clock counter
reg clockCountEn;

// Keep track of the correct number of clock cycles required for 
always @(posedge gpuClock or negedge reset) begin
    if (!reset) begin
        state <= state_reset;
        clockCounter <= 5'd0;
        clockCountEn <= 1'b0;
        doneRam <= 1'b0;
    end
    else begin
        // Increment clock counter
        if (clockCountEn) clockCounter <= clockCounter + 1;
        else clockCounter <= 5'd0;
        
        // State machine for controlling RAM accesses
        case (state)
        
        state_reset:
            // New pipeline data, determine if controller is reading/writing
            if (controllerReadEn) begin
                // Controller reads take priority
                state <= state_readController;
            end
            else begin
                if (controllerWriteEn) begin
                    // Controller writes next state
                    state <= state_write;
                end
                else begin
                    // Pipeine is always reading
                    state <= state_readPipeline;
                end    
            end

            clockCountEn <= 1'b0;
            doneRam <= 1'b0;

        state_readController:
            // After done reading for the controller,
            // read for the pipeline
            state <= state_readPipeline;

            doneRam <= 1'b0;
            clockCountEn <= 1'b0;

        state_readPipeline:
            // As long as pipeline clock's prior falling edge occured before
            // this state is reached, there should be no error

            // Stall in this state until pipeline advances
            if (pipelineClock) state <= state_reset;
            else state <= state_readPipeline;

            // After this cycle, RAM controller will be done
            doneRam <= 1'b1;
            clockCountEn <= 1'b0;

        state_write:
            // Clock counter indicates the number of cycles that have alrready occured
            if (clockCounter < 5) state <= state_write;
            else state <= state_readPipeline;

            doneRam <= 1'b0;
            clockCountEn <= 1'b1;

        endcase
    end
end

// Support block reads/writes (Save last block read/write address)

// Have a lookup table of layer ID's and start addresses


endmodule