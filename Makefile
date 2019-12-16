#
# Common Makefile for the PX4 bootloaders
#

#
# Paths to common dependencies
#
export BL_BASE		?= $(wildcard .)
export LIBOPENCM3	?= $(wildcard libopencm3)

#
# Tools
#
export CC	    = arm-none-eabi-gcc
export OBJCOPY	= arm-none-eabi-objcopy

export BOARD ?= meowbit
-include boards/$(BOARD)/board.mk

# Default to F401; override in board.mk if needed
FN ?= f4
CPUTYPE ?= STM32F401
CPUTYPE_SHORT ?= STM32F4
CPUFLAGS ?= -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
# f1: -mcpu=cortex-m3
# f3: as f4
# f7: -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16

LINKER_FILE ?= linker/stm32$(FN).ld 
EXTRAFLAGS ?= -D$(CPUTYPE)

#
# Common configuration
#
export FLAGS		 = -std=gnu99 \
			   -Os \
			   -g \
			   -Wundef \
			   -Wall \
			   -fno-builtin \
			   -I$(LIBOPENCM3)/include \
			   -Iboards/$(BOARD) \
			   -ffunction-sections \
			   -nostartfiles \
			   -lnosys \
			   -Wl,-gc-sections \
			   -Wl,-g \
			   -Wno-unused \
			   -Werror

export COMMON_SRCS	 = bl.c test.bmp.c empty.bmp.c ghostfat.c dmesg.c screen.c images.c settings.c hf2.c support.c util.c flashwarning.c


SRCS		 = $(COMMON_SRCS) main.c

OBJS		:= $(patsubst %.c,%.o,$(SRCS))
DEPS		:= $(OBJS:.o=.d)


FLAGS		+= -mthumb $(CPUFLAGS) \
       -D$(CPUTYPE_SHORT) \
       -T$(LINKER_FILE) \
	   -L$(LIBOPENCM3)/lib \
	   -lopencm3_stm32$(FN) \
        $(EXTRAFLAGS)

#
# Bootloaders to build
#

all:	build-bl sizes

clean:
	cd libopencm3 && make --no-print-directory clean && cd ..
	rm -f *.elf *.bin # Remove any elf or bin files contained directly in the Bootloader directory
	rm -rf build # Remove build directories

# any file generated during libopencm3 build
OCM3FILE = libopencm3/include/libopencm3/stm32/f4/nvic.h

build-bl: $(MAKEFILE_LIST) $(OCM3FILE) do-build

#
# General rules for making dependency and object files
# This is where the compiler is called
#
#
# Common rules for makefiles for the PX4 bootloaders
#

BUILD_DIR	 = build/$(BOARD)

OBJS		:= $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS)))
DEPS		:= $(OBJS:.o=.d)

ELF		 = $(BUILD_DIR)/bootloader.elf
BINARY		 = $(BUILD_DIR)/bootloader.bin
UF2		 = $(BUILD_DIR)/flasher.uf2

FL_OBJS = $(addprefix $(BUILD_DIR)/, test.bmp.o empty.bmp.o flasher.o main-flasher.o util.o dmesg.o screen.o images.o settings.o)

do-build:		$(BUILD_DIR) $(ELF) $(BINARY) $(UF2)

# Compile and generate dependency files
$(BUILD_DIR)/%.o:	%.c
	@echo Generating object $@
	$(CC) -c -MMD $(FLAGS) -o $@ $*.c

# Compile and generate dependency files
$(BUILD_DIR)/%-flasher.o:	%.c
	@echo Generating object $@
	$(CC) -c -MMD $(FLAGS) -o $@ $<

# Make the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ELF):		$(OBJS) $(MAKEFILE_LIST)
	$(CC) -o $@ $(OBJS) $(FLAGS) -Wl,-Map=$(ELF).map

$(BINARY):	$(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BINARY)

$(UF2): $(FL_OBJS) $(BINARY)
	$(CC) -o $(BUILD_DIR)/flasher.elf $(FL_OBJS) $(FLAGS:.ld=-flasher.ld)
	$(OBJCOPY) -O binary $(BUILD_DIR)/flasher.elf $(BUILD_DIR)/flasher.bin
	python uf2/utils/uf2conv.py -c -f 0x57755a57 -b 0x08010000 $(BUILD_DIR)/flasher.bin -o $(BUILD_DIR)/flasher.uf2
	$(CC) -o $(BUILD_DIR)/flasher16.elf $(FL_OBJS) $(FLAGS:.ld=-flasher16.ld)
	$(OBJCOPY) -O binary $(BUILD_DIR)/flasher16.elf $(BUILD_DIR)/flasher16.bin
	python uf2/utils/uf2conv.py -c -f 0x57755a57 -b 0x08008000 $(BUILD_DIR)/flasher16.bin -o $(BUILD_DIR)/flasher16.uf2

# Dependencies for .o files
-include $(DEPS)

flash: upload
burn: upload
b: burn
f: flash

upload: build-bl flash-bootloader


BMP = $(shell ls -1 /dev/cu.usbmodem*1 | head -1)
BMP_ARGS = -ex "target extended-remote $(BMP)" -ex "mon tpwr enable" -ex "mon swdp_scan" -ex "attach 1"
GDB = arm-none-eabi-gdb

#
# Show sizes
#
.PHONY: sizes
sizes:
	@-find build/*/ -name '*.elf' -type f | xargs size 2> /dev/null || :

#
# Submodule management
#

$(OCM3FILE): 
	${MAKE} checksubmodules
	${MAKE} -C $(LIBOPENCM3) -j10 lib

.PHONY: checksubmodules
checksubmodules: updatesubmodules
	$(Q) ($(BL_BASE)/Tools/check_submodules.sh)

.PHONY: updatesubmodules
updatesubmodules:
	$(Q) (git submodule init)
	$(Q) (git submodule update)

