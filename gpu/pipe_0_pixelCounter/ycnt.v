module ycnt
(
    input pixelInc, // Pulse for a new pixel
    input unsigned [10:0] x, // Looking for x position to change to change y position
    input reset, // 0 = reset, 1 = no reset
    output reg [10:0] y // Y is value from 0 to 1919 (1920 pixels)
);

parameter X_MAX = 1280;
parameter Y_MAX = 720;


initial y = 0;

// For every new pixel, determine what new y value is
always @(posedge pixelInc or negedge reset) begin
    if (!reset) begin
       y <= 0;
    end else begin
		  // X just changed to 0 on same clock edge, will show as max x value now
        // for new row, increment Y
        if (x == X_MAX - 1) begin
            // New row, either increment Y or reset it
            if (y + 1 < Y_MAX) y <= y + 1;
				else y <= 0;
        end
		  else y <= y;
    end
end

endmodule