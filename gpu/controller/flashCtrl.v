module flashCtrl(
    input [4:0] command, // Current command[15:11] bits
    output rstFlash, // Reset flash
    output rFlash, // Read flash enable
    output wFlash // Write flash enable
);

// Must specifically target flash to reset it to protect the glyph data
assign rstFlash = !(command[4:0] == 5'b11100); 
assign rFlash = (command[4:0] == 5'b01100); // read Flash enable
assign wFlash = (command[4:0] == 5'b10100); // Write Flash enanble

endmodule