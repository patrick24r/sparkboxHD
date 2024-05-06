# SparkboxHD
Sparkbox, now in 720p!  
The SparkboxHD is a device that will support 2D graphics
                      
### Build and Run
Build using

```
python3 build_sparkbox.py <device>
```

where `<device>` is an existing directory at `device/<device>`.


## MCU
User controller - 2 Analog sticks, 16 Buttons 

### OS
The OS allows the user to choose a level to load

### Levels

#### Loading a level
A level can only be as large as what can fit in memory

Audio data is decompressed and loaded to internal RAM
Video data is loaded to the GPU