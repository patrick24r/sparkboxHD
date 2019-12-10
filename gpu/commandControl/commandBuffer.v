module commandBuffer(
    input gpuClock,
    input reset,
    input gpuBusyController, // gpu busy signal from controller
    input frameRendering, // 1 = frame rendering, 0 = frame not rendering
    input [15:0] interfaceCmd, // Command received from interface
    input [15:0] interfaceData, // Data received from interface
    input [15:0] dataFromGpu, // Data read from GPU
    output reg gpuBusy, // Determination if GPU is busy or not
    output reg [15:0] gpuCommand, // Command to send to GPU
    output reg [15:0] gpuData // Data to send to GPU
);

parameter   CASE_IDLE = 2'b00,
            CASE_RENDERING = 2'b01,
            CASE_POSTRENDER = 2'b11;

parameter   POST_READ_FLAGS = 4'h0,
            POST_READ_XPOSITION = 4'h1,
            POST_READ_YPOSITION = 4'h2,
            POST_READ_XVELOCITY = 4'h3,
            POST_READ_YVELOCITY = 4'h4,
            POST_READ_CURRFRAME = 4'h5,
            POST_WRITE_XPOSITION = 4'h6,
            POST_WRITE_YPOSITION = 4'h7,
            POST_WRITE_FRAME_NUM = 4'h8;

parameter CMD_UPDATE_FRAME = 16'd1;

// Register for top level state machine
reg [1:0] state;
// Registers for postrender operations
reg postrender_state;
reg unsigned [4:0] layerCounter;
reg [127:0] headerData;

initial begin
    state = CASE_IDLE;
    postrender_state = POST_READ_FLAGS;
    headerData = 0;
    layerCounter = 0;
end



always @(posedge gpuClock or negedge reset) begin
if (!reset) begin
    // Reset outputs
    gpuBusy <= 0;
    gpuCommand <= 0;
    gpuData <= 0;

    // reset internal registers
    state <= CASE_IDLE;
    postrender_state <= POST_READ_FLAGS;
    layerCounter <= 0;
    headerData <= 0;
end
else begin
    case (state)

    CASE_IDLE: begin
        // Not currently rendering, accepting commands from interface 
        gpuBusy <= gpuBusyController;

        // Do not impede flow of command and data if GPU isn't busy
		  if (gpuBusyController) begin
				gpuCommand <= interfaceCmd;
				gpuData <= interfaceData;
		  end else begin
				gpuCommand <= 16'd0;
				gpuData <= 16'd0;
		  end


        if (interfaceCmd == CMD_UPDATE_FRAME) state <= CASE_RENDERING;
        else state <= CASE_IDLE;
        postrender_state <= POST_READ_FLAGS;
        layerCounter <= 0;
        headerData <= 0;
    end

    CASE_RENDERING: begin
        gpuBusy <= 1'b1;

        gpuCommand <= 0;
        gpuData <= 0;

        if (!frameRendering) state <= CASE_POSTRENDER;
        else state <= CASE_RENDERING;
        postrender_state <= POST_READ_FLAGS;
        layerCounter <= 0;
        headerData <= 0;
    end

    CASE_POSTRENDER: begin
        gpuBusy <= 1'b1;
        
        case (postrender_state)
        // Read layer header - flags, x position, y position, x velocity, y velocity, current frame #
        POST_READ_FLAGS: begin
            // GPU command - read layer headers - register 0, layer layerCounter
            gpuCommand <= {7'b0100100, 3'b000, 1'b0, layerCounter};
            gpuData <= 0;
            // Advance to next postrender state
            postrender_state <= POST_READ_XPOSITION;
            state <= CASE_POSTRENDER;
            layerCounter <= layerCounter;
            // Reset header data
            headerData <= 0;
        end

        POST_READ_XPOSITION: begin
            // GPU command - read layer headers - register 3, layer layerCounter
            gpuCommand <= {7'b0100100, 3'b011, 1'b0, layerCounter};
            gpuData <= 0;
            // Advance to next postrender state
            postrender_state <= POST_READ_YPOSITION;
            state <= CASE_POSTRENDER;
            layerCounter <= layerCounter;
            // Save layer header flags from previous state
            headerData[15:0] <= dataFromGpu;
        end

        POST_READ_YPOSITION: begin
            // GPU command - read layer headers - register 4, layer layerCounter
            gpuCommand <= {7'b0100100, 3'b100, 1'b0, layerCounter};
            gpuData <= 0;
            // Advance to next postrender state
            postrender_state <= POST_READ_XVELOCITY;
            state <= CASE_POSTRENDER;
            layerCounter <= layerCounter;
            // Save x position flags from previous state
            headerData[63:48] <= dataFromGpu;
        end

        POST_READ_XVELOCITY: begin
            // GPU command - read layer headers - register 5, layer layerCounter
            gpuCommand <= {7'b0100100, 3'b101, 1'b0, layerCounter};
            gpuData <= 0;
            // Advance to next postrender state
            postrender_state <= POST_READ_YVELOCITY;
            state <= CASE_POSTRENDER;
            layerCounter <= layerCounter;
            // Save y position from previous state
            headerData[79:64] <= dataFromGpu;
        end

        POST_READ_YVELOCITY: begin
            // GPU command - read layer headers - register 6, layer layerCounter
            gpuCommand <= {7'b0100100, 3'b110, 1'b0, layerCounter};
            gpuData <= 0;
            // Advance to next postrender state
            postrender_state <= POST_READ_CURRFRAME;
            state <= CASE_POSTRENDER;
            layerCounter <= layerCounter;
            // Save x velocity from previous state
            headerData[95:80] <= dataFromGpu;
        end 

        POST_READ_CURRFRAME: begin
            // GPU command - read layer headers - register 7, layer layerCounter
            gpuCommand <= {7'b0100100, 3'b111, 1'b0, layerCounter};
            gpuData <= 0;
            // Advance to next postrender state
            postrender_state <= POST_WRITE_XPOSITION;
            state <= CASE_POSTRENDER;
            layerCounter <= layerCounter;
            // Save y velocity from previous state
            headerData[111:96] <= dataFromGpu;
        end

        // Write back new data - new x position, new y position, new frame number
        POST_WRITE_XPOSITION: begin
            // GPU command - write layer headers - register 3, layer layerCounter
            gpuCommand <= {7'b1000100, 3'b011, 1'b0, layerCounter};
            // Write back original value if layer is not a sprite, otherwise add x velocity
            gpuData <= headerData[1] ? headerData[63:48] + headerData[95:80]: headerData[63:48];
            // Advance to next postrender state
            postrender_state <= POST_WRITE_YPOSITION;
            state <= CASE_POSTRENDER;
            layerCounter <= layerCounter;
            // Save layer header flags from previous state
            headerData[127:112] <= dataFromGpu;
        end

        POST_WRITE_YPOSITION: begin
            // GPU command - write layer headers - register 4, layer layerCounter
            gpuCommand <= {7'b1000100, 3'b100, 1'b0, layerCounter};
            // Write back original value if layer is not a sprite, otherwise add y velocity
            gpuData <= headerData[1] ? headerData[79:64] + headerData[111:96] : headerData[79:64];
            // Advance to next postrender state
            postrender_state <= POST_WRITE_FRAME_NUM;
            state <= CASE_POSTRENDER;
            layerCounter <= layerCounter;
            headerData <= headerData;
        end

        POST_WRITE_FRAME_NUM: begin
            // GPU command - write layer headers - register 7, layer layerCounter
            gpuCommand <= {7'b1000100, 3'b111, 1'b0, layerCounter};
            // Write back original value if layer is not a sprite
            // If layer is a sprite, bound check the new frame number against
            // total number of frames
            gpuData <=  headerData[1] ? 
                            // If new frame number is less than total number of frames, allow it
                            ( ($unsigned(headerData[127:120] + 1) < $unsigned(headerData[119:112])) ? 
                            {headerData[127:120] + 1, headerData[119:112]} 
                            // Otherwise, reset current frame number to 0
                            :{8'd0, headerData[119:112]} ) 
                        :headerData[127:112];

            // Advance to next layer or set state to idle
            headerData <= 0;
            layerCounter <= layerCounter + 1;
            postrender_state <= POST_READ_FLAGS;
            // Done postrender if current layer is the last one
            if (layerCounter == 5'b11111) state <= CASE_IDLE; 
            else state <= CASE_POSTRENDER;
        end

        default: begin

        end

        endcase

    end

    default: begin
        state <= CASE_IDLE; // Internal registers
        postrender_state <= POST_READ_FLAGS;
        layerCounter <= 0;
        headerData <= 0;

        // Output 
        gpuBusy <= gpuBusyController;
        gpuCommand <= interfaceCmd;
        gpuData <= interfaceData;
    end
    endcase

end
end

endmodule