##########################################################################################################################
# Sifive_Hifive1 GCC compiler Makefile
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
# ------------------------------------------------

######################################
# target
######################################
TARGET = Huawei_LiteOS
######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O0 -g



#######################################
# binaries
#######################################
CROSS_COMPILE = riscv64-unknown-elf
ifeq ($(RISCV_PATH),)
RISCV_GCC     := $(CROSS_COMPILE)-gcc
RISCV_GXX     := $(CROSS_COMPILE)-g++
RISCV_OBJDUMP := $(CROSS_COMPILE)-objdump
RISCV_OBJCOPY := $(CROSS_COMPILE)-objcopy
RISCV_GDB     := $(CROSS_COMPILE)-gdb
RISCV_AR      := $(CROSS_COMPILE)-ar
RISCV_AS      := $(CROSS_COMPILE)-as
RISCV_SIZE    := $(CROSS_COMPILE)-size
RISCV_GDB     := $(CROSS_COMPILE)-gdb
else
RISCV_GCC     := $(abspath $(RISCV_PATH)/bin/$(CROSS_COMPILE)-gcc)
RISCV_GXX     := $(abspath $(RISCV_PATH)/bin/$(CROSS_COMPILE)-g++)
RISCV_OBJDUMP := $(abspath $(RISCV_PATH)/bin/$(CROSS_COMPILE)-objdump)
RISCV_OBJCOPY := $(abspath $(RISCV_PATH)/bin/$(CROSS_COMPILE)-objcopy)
RISCV_GDB     := $(abspath $(RISCV_PATH)/bin/$(CROSS_COMPILE)-gdb)
RISCV_AR      := $(abspath $(RISCV_PATH)/bin/$(CROSS_COMPILE)-ar)
RISCV_AS      := $(abspath $(RISCV_PATH)/bin/$(CROSS_COMPILE)-as)
RISCV_SIZE    := $(abspath $(RISCV_PATH)/bin/$(CROSS_COMPILE)-size)
RISCV_GDB     := $(abspath $(RISCV_PATH)/bin/$(CROSS_COMPILE)-gdb)
PATH          := $(abspath $(RISCV_PATH)/bin):$(PATH)
endif

CC         = $(RISCV_GCC)
GXX        = $(RISCV_GXX)
OBJCOPY    = $(RISCV_OBJCOPY)
OBJDUMP    = $(RISCV_OBJDUMP)
AR         = $(RISCV_AR)
AS         = $(RISCV_AS)
SZ         = $(RISCV_SIZE)


PROJECTBASE = $(abspath $(CURDIR))
TOP_DIR     = $(abspath $(PROJECTBASE)/../../..)


#######################################
# paths
#######################################
# firmware library path
PERIFLIB_PATH =

# Build path
BUILD_DIR = build

######################################
# source
######################################
# ASM
ASM_SOURCES_S =  \
        ${wildcard $(PROJECTBASE)/entry.S} \
        ${wildcard $(PROJECTBASE)/crt0.S}
# C sources

GLOSS_SRC = \
        ${wildcard $(TOP_DIR)/targets/Sifive_Hifive1/LIB/*.c}
        C_SOURCES += $(GLOSS_SRC)

Driver_SRC = \
        ${wildcard $(TOP_DIR)/drivers/third_party/Sifive/*.c} \
        ${wildcard $(TOP_DIR)/drivers/third_party/Sifive/drivers/*.c}
        C_SOURCES += $(Driver_SRC)

KERNEL_SRC =  \
        ${wildcard $(TOP_DIR)/kernel/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/core/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/ipc/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/mem/heap/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/mem/membox/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/mem/common/los_slab.c} \
        ${wildcard $(TOP_DIR)/kernel/base/mem/common/los_slabmem.c} \
        ${wildcard $(TOP_DIR)/kernel/base/mem/common/los_memcheck.c} \
        ${wildcard $(TOP_DIR)/kernel/base/misc/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/om/*.c}
        C_SOURCES += $(KERNEL_SRC)

CMSIS_SRC =  \
        ${wildcard $(TOP_DIR)/osdepends/liteos/cmsis/*.c}
        #C_SOURCES += $(CMSIS_SRC)

ARCH_SRC =  \
        ${wildcard $(TOP_DIR)/arch/riscv/*.c}
        C_SOURCES += $(ARCH_SRC)

OS_DEPENDS_SRC = \
        ${wildcard $(TOP_DIR)/osdepends/liteos/*.c}
        #C_SOURCES += $(OS_DEPENDS_SRC)

SOCKET_ADAPTER_SRC = \
        ${wildcard $(TOP_DIR)/components/net/sal/*.c}
        #C_SOURCES += $(SOCKET_ADAPTER_SRC)

NEWLIB_SRC = \
        ${wildcard $(TOP_DIR)/components/lib/libc/malloc.c}\
        ${wildcard $(TOP_DIR)/components/lib/libc/newlib_stub.c} \
	${wildcard $(TOP_DIR)/components/lib/libc/errno.c}
        #C_SOURCES += $(NEWLIB_SRC)

ATINY_LOG = \
        ${wildcard $(TOP_DIR)/components/log/*.c}
        #C_SOURCES += $(ATINY_LOG)

USER_SRC =  \
        $(TOP_DIR)/targets/Sifive_Hifive1/Src/main.c
        C_SOURCES += $(USER_SRC)

UTIL_SRC = \
        $(TOP_DIR)/utils/rbtree.c
        C_SOURCES += $(UTIL_SRC)






# ASM sources

ASM_SOURCES_S +=  \
        ${wildcard $(TOP_DIR)/arch/riscv/los_dispatch.S}



######################################
# firmware library
######################################
PERIFLIB_SOURCES =


#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4
# fpu
FPU = -mfpu=fpv4-sp-d16
# float-abi
FLOAT-ABI = -mfloat-abi=hard
# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
        -D USE_HAL_DRIVER \
        -D STM32F429xx \
        -D NDEBUG





# AS includes
AS_INCLUDES =

# C includes
HAL_DRIVER_INC = \
	-I $(TOP_DIR)/targets/Sifive_Hifive1/Inc/drivers
        #C_INCLUDES += $(HAL_DRIVER_INC)
		
INCLUDE_INC = \
        -I $(TOP_DIR)/include
        #C_INCLUDES += $(INCLUDE_INC)
		
LWM2M_ATINY_INC = \
        -I $(TOP_DIR)/components/connectivity/agent_tiny/atiny_lwm2m
        #C_INCLUDES += $(LWM2M_ATINY_INC)

HARDWARE_INC = \
        -I $(TOP_DIR)/targets/Cloud_STM32F429IGTx_FIRE/Hardware/Inc
        #C_INCLUDES += $(HARDWARE_INC)

USER_INC = \
        -I $(TOP_DIR)/targets/Cloud_STM32F429IGTx_FIRE/Inc
        #C_INCLUDES += $(USER_INC)

CMSIS_INC = \
        -I $(TOP_DIR)/arch/arm/arm-m/include \
        -I $(TOP_DIR)/arch/arm/common/cmsis
        #C_INCLUDES += $(CMSIS_INC)

LITEOS_CMSIS = \
        -I $(TOP_DIR)/osdepends/liteos/cmsis
        #C_INCLUDES += $(LITEOS_CMSIS)
		
BOARD_INC = \
        -I $(TOP_DIR)/targets/Sifive_Hifive1/Inc
        C_INCLUDES += $(BOARD_INC)

OS_CONFIG_INC = \
        -I $(TOP_DIR)/targets/Sifive_Hifive1/OS_CONFIG
        C_INCLUDES += $(OS_CONFIG_INC)

KERNEL_INC = \
        -I $(TOP_DIR)/kernel/include \
	-I $(TOP_DIR)/kernel/base/include \
	-I $(TOP_DIR)/kernel/extended/include
        C_INCLUDES += $(KERNEL_INC)

ARCH_INC = \
        -I $(TOP_DIR)/arch/riscv
        C_INCLUDES += $(ARCH_INC)

	



DEMO_INC = \
        -I $(TOP_DIR)/demos
        C_INCLUDES += $(DEMO_INC)

# Set the arch, ABI, and code model
RISCV_CFLAGS   += -march=rv32imac -mabi=ilp32 -mcmodel=medlow $(OPT)
RISCV_CXXFLAGS += -march=rv32imac -mabi=ilp32 -mcmodel=medlow $(OPT)
# Prune unused functions and data
RISCV_CFLAGS   += -ffunction-sections -fdata-sections
RISCV_CXXFLAGS += -ffunction-sections -fdata-sections
# Include the Metal headers
RISCV_CFLAGS   += $(C_INCLUDES)
RISCV_CXXFLAGS += $(C_INCLUDES)

# Turn on garbage collection for unused sections
RISCV_LDFLAGS += -march=rv32imac -mabi=ilp32 -mcmodel=medlow -ffunction-sections -fdata-sections -Wl,--gc-sections
# Turn off the C standard library
RISCV_LDFLAGS += -nostartfiles -nostdlib

# Link to the relevant libraries
RISCV_LDLIBS += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group


# compile gcc flags
ASFLAGS = -march=rv32imac -mabi=ilp32 -mcmodel=medlow -ffunction-sections -fdata-sections -O0

CFLAGS = $(RISCV_CFLAGS)
LDFLAGS = $(RISCV_LDFLAGS)

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
ASFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$@"

# excluded unnecessary warnings
CFLAGS += -Wno-missing-braces


#######################################
# LDFLAGS
#######################################
# link script
LD_FILE = SIFIVE_LiteOS.ld

LDSCRIPT = $(PROJECTBASE)/$(LD_FILE)

# libraries
LIBS = -lnosys
ifeq ($(USE_OTA), yes)
LIBS += -lhwpatch
endif
LIBDIR = -L$(PROJECTBASE)/../Lib
LDFLAGS += $(RISCV_LDLIBS)
LDFLAGS +=   -T$(LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

ifeq ($(WITH_MQTT), yes)
LDFLAGS += -u _printf_float
endif

ifneq ($(RISCV_OPENOCD_PATH),)
RISCV_OPENOCD=$(RISCV_OPENOCD_PATH)/bin/openocd
#RISCV_OPENOCD=/home/baikal/workspace/RISCV/gnu-mcu-eclipse/openocd/0.10.0-10-20181020-0522/bin/openocd
else
#if RISCV_OPENOCD_PATH is not set, just look on the PATH
RISCV_OPENOCD=openocd
endif


# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES_s:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES_s)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES_S:.S=.o)))
vpath %.S $(sort $(dir $(ASM_SOURCES_S)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS)  $< -o $@
	#$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(CC) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

upload: $(BUILD_DIR)/$(TARGET).elf
	./upload --elf $(BUILD_DIR)/$(TARGET).elf  --openocd $(RISCV_OPENOCD) --gdb $(RISCV_GDB) --openocd-config ./openocd.cfg

debug: $(BUILD_DIR)/$(TARGET).elf
	./debug --elf $(BUILD_DIR)/$(TARGET).elf --openocd $(RISCV_OPENOCD) --gdb $(RISCV_GDB) --openocd-config ./openocd.cfg


$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)

#######################################
# dependencies
#######################################
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
