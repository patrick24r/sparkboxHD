module xcnt
(
    input layer, // Looking for layer to change to change x position
    input reset, // 0 = reset, 1 = no reset
    output reg [10:0] x // X is value from 0 to 1919 (1920 pixels)
);

parameter X_MAX = 1920;

initial x = 0;

// Whenever layer changes, may need to update x
always @(posedge layer) begin
    if (!reset)
        x <= 0;
    else begin
        if (layer == 0) begin
            // Layer changed to 0, 
            if (x < X_MAX - 1)
                x <= x + 1;
            else
                x <= 0;
        end
        else begin
            // Layer changed but not reset to 0, keep X value
            x <= x;
        end
    end
end

endmodule