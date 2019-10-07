module ycnt
(
    input x, // Looking for x position to change to change y position
    input reset, // 0 = reset, 1 = no reset
    output reg [10:0] y // Y is value from 0 to 1919 (1920 pixels)
);

parameter Y_MAX = 1080;

initial y = 0;

// Whenever X changes, may need to update Y as well
always @(x or reset) begin
    if (!reset || y == Y_MAX - 1)
        y <= 0;
    else begin
		  // X changed to 0, increment Y
        if (x == 0 && y < Y_MAX - 1) y <= y + 1;
		  // X did not change to 0, keep y value
		  else y <= y;
    end
end

endmodule