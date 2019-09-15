`include "layerRegisterMem.v"

// This module interfaces and controls all layer header data
module layerHeaderInfo(
    input clk, // Master clock
    input reset, // 0 = reset, 1 = no reset
    input [4:0] readLayer, // Layer header to read
    input [4:0] writeLayer, // Layer to write data to
    input [2:0] writeLayerRegister, // Index of the layer register to write (defined in layerInfoTop.v)
    input writeEn, // 1 = write enabled, 0 = write disabled
    input [15:0] writeData, // Data to write to specified register
    output [127:0] layerInfo // All layer header info that was read
);

// 8  write enable wires for 8 layer registers
wire write0en, write1en, write2en, write3en, write4en, write5en, write6en, write7en;

// Only write to register specified by writeLayerRegister
assign write0en = (writeEn && writeLayerRegister == 3'b000);
assign write1en = (writeEn && writeLayerRegister == 3'b001);
assign write2en = (writeEn && writeLayerRegister == 3'b010);
assign write3en = (writeEn && writeLayerRegister == 3'b011);
assign write4en = (writeEn && writeLayerRegister == 3'b100);
assign write5en = (writeEn && writeLayerRegister == 3'b101);
assign write6en = (writeEn && writeLayerRegister == 3'b110);
assign write7en = (writeEn && writeLayerRegister == 3'b111);

// 8 memory modules for each layer register
layerRegisterMem reg_0(clk, reset, readLayer, write0en, writeLayer, writeData, layerInfo[15:0]);
layerRegisterMem reg_1(clk, reset, readLayer, write0en, writeLayer, writeData, layerInfo[31:15]);
layerRegisterMem reg_2(clk, reset, readLayer, write0en, writeLayer, writeData, layerInfo[47:32]);
layerRegisterMem reg_3(clk, reset, readLayer, write0en, writeLayer, writeData, layerInfo[63:48]);
layerRegisterMem reg_4(clk, reset, readLayer, write0en, writeLayer, writeData, layerInfo[79:64]);
layerRegisterMem reg_5(clk, reset, readLayer, write0en, writeLayer, writeData, layerInfo[95:80]);
layerRegisterMem reg_6(clk, reset, readLayer, write0en, writeLayer, writeData, layerInfo[111:96]);
layerRegisterMem reg_7(clk, reset, readLayer, write0en, writeLayer, writeData, layerInfo[127:112]);




endmodule