#
# Makefile for STM32H7 family
#
#######################################
# Usage
#######################################
#
# [make]
# [make all]
# [make clean]
# [make flash]
#
TARGETDIR = 
TARGET = consoleStartup

PROCESSOR = STM32H743xx

#######################################
# Defines
#######################################
# Define Compilers
PREFIX = arm-none-eabi
CC = $(PREFIX)-gcc
CPP = $(PREFIX)-g++
AS = $(PREFIX)-gcc
CP = $(PREFIX)-objcopy
OD = $(PREFIX)-objdump
SZ = $(PREFIX)-size


# Define Optimization Level
# O0 -> No optimization
# O1, O2, O3 -> Speed optimizations
# Os -> Size optimization
# Og -> Optimize for debugging
OPT = -Os

# Define directories
SRCDIR := src
INCDIR := inc
OBJDIR := obj
COMDIR := ../common

# Find all source files
SRC := $(shell find $(SRCDIR) $(COMDIR) -name [^.]*.c)
SRC += $(shell find $(SRCDIR) $(COMDIR) -name [^.]*.s)
SRC += $(shell find $(SRCDIR) $(COMDIR) -name [^.]*.cpp)
SRCDIR := $(sort $(dir $(SRC)))

# Find all include directories
INC := $(sort $(dir $(shell find $(INCDIR) $(COMDIR) -name [^.]*.h)))
INC += $(sort $(dir $(shell find $(INCDIR) $(COMDIR) -name [^.]*.hpp)))

# Define vpaths
vpath %.c $(SRCDIR)
vpath %.cpp $(SRCDIR)
vpath %.s $(SRCDIR)
vpath %.o $(OBJDIR)

# Generate objects
OBJ := $(addprefix $(OBJDIR)/,$(notdir $(SRC)))
OBJ := $(OBJ:.c=.o)
OBJ := $(OBJ:.cpp=.o)
OBJ := $(OBJ:.s=.o)

#######################################
# LDFLAGS
#######################################
MCU = --specs=nano.specs -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard
# link script
LDSCRIPT = STM32H743ZITx_FLASH.ld

# libraries
LDFLAGS = $(MCU) -T$(LDSCRIPT) -Wl,-Map=$(TARGET).map -Wl,--gc-sections -static -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group

#######################################
# Compiler Flags
#######################################

INCFLAGS := $(addprefix -I, $(INC))

SRCFLAGS = $(MCU) -c $(INCFLAGS) $(OPT) -g3 -DDEBUG -Wall -fdata-sections -ffunction-sections \
	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" 
ASFLAGS = $(SRCFLAGS) -x assembler-with-cpp
CFLAGS = $(SRCFLAGS) -DUSE_HAL_DRIVER -std=gnu17 -fstack-usage
CPPFLAGS = $(SRCFLAGS) -DUSE_HAL_DRIVER -std=gnu++14 -fstack-usage -fno-rtti -fno-use-cxa-atexit -fexceptions

#######################################
# Targets
#######################################
all: $(TARGET).hex $(TARGET).bin
	@echo Compilation complete!

# Objcopy for binary
$(TARGET).bin: $(TARGET).elf
	$(CP) -O binary $< $@

# Objcopy for hex
$(TARGET).hex: $(TARGET).elf
	$(CP) -O ihex $^ $@

# Compile executable
$(TARGET).elf: $(OBJ)
	$(CPP) $(LDFLAGS) $^ -o $@
	$(SZ) $@

# Compile c objects rule
$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@ 

# Compile c++ objects rule
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CPP) -c $(CPPFLAGS) $< -o $@

# Compile s objects rule
$(OBJDIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

#######################################
# Clean
#######################################
clean:
	rm -rf $(OBJDIR) $(TARGET).elf $(TARGET).bin $(TARGET).hex $(TARGET).map


#######################################
# Flash
#######################################
flash: $(TARGET).bin
	/mnt/c/Windows/System32/cmd.exe /C powershell -Command \
	"Copy-Item $< (Get-WMIObject Win32_Volume \
	| ? {\$$_.Label -eq 'NOD_H743ZI2'} | %{\$$_.DriveLetter})"

#######################################
# Debug
#######################################
debug: flash
	st-util -p 4242 &
	gdb-multiarch $(TARGET).elf -tui -ex "target remote 127.0.0.1:4242"