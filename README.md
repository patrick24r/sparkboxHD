# SparkboxHD
Sparkbox, now in 720p!  
The SparkboxHD is a device that will support 2D graphics
                      
Insert SparkboxHD functional block diagram here

## Controller
User controller - 2 Analog sticks, 16 Buttons 

## CPU
The SparkboxHD CPU is a STM32F4 microcontroller and is the "brains" of the device. It recevies input from the user through the controller and microSD card (game cartridge). It produces an I2S audio output, controls the SparkboxHD GPU for video output, and can save game data to the microSD card.

## GPU
The SparkboxHD GPU is a 6 stage pipeline processor implemented on a Cyclone IV FPGA. It supports 2D graphics with up to 32 active layers and total layers limited to X MB. It also has a built in font engine.

## Pipeline
Each 'layer' is classified as either a 'Sprite', an arbitrary animated item, or 'Text', ascii character data. 

The pipelines in the GPU are listed below.  
1. Pixel Counter
2. Layer Headers
3. ALU
4. SDRAM
5. Flash
6. Palette

### 1. Pixel Counter
This pipe determines the next [Layer, X, Y] combination to find.

### 2. Layer Headers
This pipe contains information on the 32 active layers including x/y position

### 3. ALU
This pipe determines address offsets for the upcoming SDRAM and Flash pipes. This includes scaling the desired font width and height to the amount stored in memory

### 4. SDRAM
This pipe interfaces with SDRAM to read and write layer data. For text, ascii data is stored. For sprites, pixel data is stored in the form of palette indices. This pipe contains a small cache of data read in burst reads to improve average memory access time.

### Flash
This pipe contains pixel data for every supported font. In the case that the current layer in this pipe is a text layer, flash will be read. This pipe contains a small cache of data read to improve average memory access time.

### Palette
This pipe contains palette data, up to 31 colors for each layer. This pipe takes the index of the palette location read from memory (SDRAM for Sprites, Layer Headers and Flash for Text) and determines if a pixel is found and a frame is finished. This block outputs a 24 bit pixel bus and accompanying signals to interface with the HDMI transmitter.
