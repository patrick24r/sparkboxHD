// Physical interface for the LCD
interface lcdBus();
  logic [23:0] rgb; // rgb data pins
  logic d_clk; // dclk pin
  logic disp_en; // display enable pin
  logic hsync; // hsync pin
  logic vsync; // vsync pin
  logic d_en; // data enable pin
  
  // For use on the fpga
  modport controller(
	output rgb, 
	output d_clk, 
	output disp_en, 
	output hsync, 
	output vsync, 
	output d_en);
	
  // Debug use
  modport debug(
	input rgb, 
	input d_clk, 
	input disp_en, 
	input hsync, 
	input vsync, 
	input d_en);
	
endinterface