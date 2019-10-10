module xcnt(
    input pixelInc, // Looking for pulse to indicate new pixel
    input reset, // 0 = reset, 1 = no reset
    output reg unsigned [10:0] x // X is value from 0 to 1919 (1920 pixels)
);

parameter X_MAX = 1280;

initial x = 0;

// Whenever layer changes, may need to update x
always @(posedge pixelInc or negedge reset) begin
    if (!reset)
        x <= 0;
    else begin
        // Update x value
        if (x < X_MAX - 1) x <= x + 1;
        else x <= 0;
    end
end

endmodule