module ramCtrl(
    input [4:0] command, // Current command[15:11}] bits
    output rstRAM, // reset RAM
    output rRAM, // read RAM enable
    output wRAM // write RAM enable
);

// RAM control outputs
assign rstRAM = !(command[4:0] == 5'b11000 || command[4:0] == 5'b11010); // Reset RAM
assign rRAM = (command[4:0] == 5'b01010); // read RAM enable
assign wRAM = (command[4:0] == 5'b10010); // Write RAM enable

endmodule