`define Y_MAX 1080

module ycnt
(
    input x, // Looking for x position to change to change y position
    input reset, // 0 = reset, 1 = no reset
    output reg [10:0] y // Y is value from 0 to 1919 (1920 pixels)
);

initial y = 0;

// Whenever X changes, may need to update Y as well
always @(x) begin
    if (!reset)
        y <= 0;
    else begin
        if (layer == 0) begin
            // X changed to 0, increment Y
            if (y < Y_MAX - 1)
                y <= y + 1;
            else
                y <= 0;
        end
        else begin
            // X changed but is now not 0, do not change Y
            y <= y;
        end
    end
end

endmodule