//`include "layerRegisterMem.v"

// This module interfaces and controls all layer header data
module layerHeaderInfo(
    input clk, // GPU clock
    input reset, // 0 = reset, 1 = no reset
    input resetLayerEn, // 0 = reset layer, 1 = no reset layer 
    input [4:0] readLayerPipe, // Layer header to read for pipeline
    input [4:0] readWriteLayerCtrl, // Layer header to read for controller
    input [2:0] ctrlLayerRegister, // Index of the layer register to write/read (only used by controller and defined in layerInfoTop.sv)
    input ctrlWriteEn, // 1 = write enabled, 0 = write disabled
    input [15:0] ctrlWriteData, // Data to write to specified register
    output [127:0] layerInfo, // All layer header info that was read (for pipeline)
    output reg [15:0] ctrlReadData // Data controller is reading (for controller)
);
// Layer to which to write data
reg [4:0] writeLayer;
// Write enable wires for each register
reg write0en, write1en, write2en, write3en, write4en, write5en, write6en, write7en;
// Write data wires for each register
reg [15:0] writeData;
// Controller read data
wire [15:0] ctrlRead0;
wire [15:0] ctrlRead1;
wire [15:0] ctrlRead2;
wire [15:0] ctrlRead3;
wire [15:0] ctrlRead4;
wire [15:0] ctrlRead5;
wire [15:0] ctrlRead6;
wire [15:0] ctrlRead7;

// 8 memory modules for each layer register
layerRegisterMem reg_inst_0(clk, reset, readLayerPipe, readWriteLayerCtrl, write0en, writeLayer, writeData, layerInfo[15:0], ctrlRead0);
layerRegisterMem reg_inst_1(clk, reset, readLayerPipe, readWriteLayerCtrl, write1en, writeLayer, writeData, layerInfo[31:16], ctrlRead1);
layerRegisterMem reg_inst_2(clk, reset, readLayerPipe, readWriteLayerCtrl, write2en, writeLayer, writeData, layerInfo[47:32], ctrlRead2);
layerRegisterMem reg_inst_3(clk, reset, readLayerPipe, readWriteLayerCtrl, write3en, writeLayer, writeData, layerInfo[63:48], ctrlRead3);
layerRegisterMem reg_inst_4(clk, reset, readLayerPipe, readWriteLayerCtrl, write4en, writeLayer, writeData, layerInfo[79:64], ctrlRead4);
layerRegisterMem reg_inst_5(clk, reset, readLayerPipe, readWriteLayerCtrl, write5en, writeLayer, writeData, layerInfo[95:80], ctrlRead5);
layerRegisterMem reg_inst_6(clk, reset, readLayerPipe, readWriteLayerCtrl, write6en, writeLayer, writeData, layerInfo[111:96], ctrlRead6);
layerRegisterMem reg_inst_7(clk, reset, readLayerPipe, readWriteLayerCtrl, write7en, writeLayer, writeData, layerInfo[127:112], ctrlRead7);

// Define logic based on individual layer resets and 
always begin
    if (!resetLayerEn) begin
        // Controller wants to reset an individual layer, write all 0's to 
        // that layer's registers
        writeLayer <= readWriteLayerCtrl;
        // Write 0
        writeData <= 16'd0;
        // Write to every register in that layer
        write0en <= 1;
        write1en <= 1;
        write2en <= 1;
        write3en <= 1;
        write4en <= 1;
        write5en <= 1;
        write6en <= 1;
        write7en <= 1;

    end else begin
        // Not resetting a layer, normal operation
        writeLayer <= readWriteLayerCtrl;
        // Write controller data
        writeData <= ctrlWriteData;
        // Only enable writing to the register specified by ctrlLayerRegister
        write0en <= (ctrlWriteEn && ctrlLayerRegister == 3'b000);
        write1en <= (ctrlWriteEn && ctrlLayerRegister == 3'b001);
        write2en <= (ctrlWriteEn && ctrlLayerRegister == 3'b010);
        write3en <= (ctrlWriteEn && ctrlLayerRegister == 3'b011);
        write4en <= (ctrlWriteEn && ctrlLayerRegister == 3'b100);
        write5en <= (ctrlWriteEn && ctrlLayerRegister == 3'b101);
        write6en <= (ctrlWriteEn && ctrlLayerRegister == 3'b110);
        write7en <= (ctrlWriteEn && ctrlLayerRegister == 3'b111);

    end
end

// Mux out controller read data based on register index
always begin
    case (ctrlLayerRegister)
        3'b000: ctrlReadData <= ctrlRead0;
        3'b001: ctrlReadData <= ctrlRead1;
        3'b010: ctrlReadData <= ctrlRead2;
        3'b011: ctrlReadData <= ctrlRead3;
        3'b100: ctrlReadData <= ctrlRead4;
        3'b101: ctrlReadData <= ctrlRead5;
        3'b110: ctrlReadData <= ctrlRead6;
        3'b111: ctrlReadData <= ctrlRead7;
    endcase
end

endmodule