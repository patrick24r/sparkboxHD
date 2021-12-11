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
AS = $(PREFIX)-as
CP = $(PREFIX)-objcopy
OD = $(PREFIX)-objdump
SZ = $(PREFIX)-size


# Define Optimization Level
# O0 -> No optimization
# O1, O2, O3 -> Speed optimizations
# Os -> Size optimization
# Og -> Optimize for debugging
OPT = -O0

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
# link script
LDSCRIPT = STM32H743ZITx_FLASH.ld

# libraries
LDFLAGS = -specs=nano.specs -T$(LDSCRIPT) -lc -lm -lnosys -Wl,-Map=$(TARGET).map,--cref -Wl,--gc-sections

#######################################
# Compiler Flags
#######################################
INCFLAGS := $(addprefix -I, $(INC))
CFLAGS = $(OPT) $(INCFLAGS) -Wall -fdata-sections -ffunction-sections \
	-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard \
	-DUSE_HAL_DRIVER -D$(PROCESSOR) -g -gdwarf-2

CPPFLAGS = $(CFLAGS) -fexceptions

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
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Compile c objects rule
$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile c++ objects rule
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -c -o $@ $<

#Compile s objects rule
$(OBJDIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

#######################################
# Clean
#######################################
clean:
	rm -rf $(OBJDIR) $(TARGET).elf $(TARGET).bin $(TARGET).hex $(TARGET).map $(OBJDIR)/*.o
