#pragma once
//#include "stm32xxxxx"

// This class contains
class PeripheralHandles {
public:
    // Constructor
    PeripheralHandles(){
        // Allocate space for peripheral handles
        // Use 'new' keyword to allocate the memory
        // e.g.
        // clk_handle = new clock_handle;
        // timer6_handle = new timer6handle;

        // Initialize the chip
	    // Initialize clock
	    // Initialize internal peripherals (Timers, DMA, etc.)
	    // Initialize other external hardware
	    // Clear flash memory section that may contain old level code
    }

    ~PeripheralHandles(){
        // Free peripheral handles memory
        // Use 'delete' keyword to deallocate memory
        // e.g. 
        // delete clk_handle;
    }

    // FATFS handle
    
    // Systick handle

    // GPIO handles

    // I2S handle for audio

}