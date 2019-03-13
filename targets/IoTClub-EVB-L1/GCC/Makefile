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
PREFIX    = arm-none-eabi-
CC        = $(PREFIX)gcc
AS        = $(PREFIX)gcc -x assembler-with-cpp
OBJCOPY   = $(PREFIX)objcopy
OBJDUMP   = $(PREFIX)objdump
AR        = $(PREFIX)ar
SZ        = $(PREFIX)size
LD        = $(PREFIX)ld
HEX       = $(OBJCOPY) -O ihex
BIN       = $(OBJCOPY) -O binary -S

#######################################


PROJECTBASE = $(abspath $(CURDIR))
TOP_DIR_TMP = $(abspath $(PROJECTBASE)/../../..)
TOP_DIR     = $(abspath $(TOP_DIR_TMP))


# firmware library path
PERIFLIB_PATH =

# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
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
        ${wildcard $(TOP_DIR)/kernel/base/om/*.c} \
        ${wildcard $(TOP_DIR)/kernel/extended/tickless/*.c}
        C_SOURCES += $(KERNEL_SRC)
CMSIS_SRC =  \
        ${wildcard $(TOP_DIR)/osdepends/liteos/cmsis/*.c}
        C_SOURCES += $(CMSIS_SRC)

ARCH_SRC =  \
        ${wildcard $(TOP_DIR)/arch/arm/arm-m/src/*.c}
        C_SOURCES += $(ARCH_SRC)

UTIL_SRC =  \
        ${wildcard $(TOP_DIR)/utils/*.c} \
        $(TOP_DIR)/components/lib/libc/malloc.c
        C_SOURCES += $(UTIL_SRC)

#AT FRAMEWORK
AT_SRC =  \
        ${wildcard $(TOP_DIR)/components/at/*.c}
        C_SOURCES += $(AT_SRC)
#driver hal
DIRVERHAL_SRC =  \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c              \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c          \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c       \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c       \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac.c          \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac_ex.c       \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c          \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c       \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c        \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c     \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c         \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c          \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c       \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c          \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c       \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c          \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c       \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c          \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c       \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c         \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c      \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_usart.c        \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_usart_ex.c

        C_SOURCES += $(DIRVERHAL_SRC)

#SHELL FRAMEWORK
SHELL_SRC =  \
        ${wildcard $(TOP_DIR)/components/shell/*.c}
        C_SOURCES += $(SHELL_SRC)

#OS_PORT COMPONENTS
OSPORT_SRC =  \
        ${wildcard $(TOP_DIR)/components/osport/*.c}
        C_SOURCES += $(OSPORT_SRC)

#DRIVER_FRAME COMPONENTS
DIRVERFRAME_SRC =  \
        $(TOP_DIR)/components/driver/los_dev.c
        C_SOURCES += $(DIRVERFRAME_SRC)
        
#project applications source
#device driver
DEVICEDRIVER_SRC =  \
        $(TOP_DIR)/targets/IoTClub-EVB-L1/DeviceDriver/NB/nb.c            \
        $(TOP_DIR)/targets/IoTClub-EVB-L1/DeviceDriver/BH1750/BH1750.c    \
        $(TOP_DIR)/targets/IoTClub-EVB-L1/DeviceDriver/DHT11/DHT11_BUS.c

        C_SOURCES += $(DEVICEDRIVER_SRC)
#common hardware
HARDWARE_SRC =  \
        ${wildcard $(TOP_DIR)/targets/IoTClub-EVB-L1/Hardware/*.c}
        C_SOURCES += $(HARDWARE_SRC)

#startup code and applications
USR_SRC =       \
        ${wildcard $(TOP_DIR)/targets/IoTClub-EVB-L1/Usr/*.c}   \
        ${wildcard $(TOP_DIR)/targets/IoTClub-EVB-L1/Apps/*.c}
        C_SOURCES += $(USR_SRC)


# ASM sources
ASM_SOURCES_s =  \
        ${wildcard $(TOP_DIR)/targets/IoTClub-EVB-L1/Usr/los_startup_gcc.s}

ASM_SOURCES_S =  \
        ${wildcard $(TOP_DIR)/arch/arm/arm-m/armv7-m/gcc/los_dispatch.S}

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
        -DUSE_HAL_DRIVER \
        -DSTM32L431xx

# AS includes
AS_INCLUDES =

# C includes
KERNEL_INC = \
        -I $(TOP_DIR)/kernel/include \
        -I $(TOP_DIR)/kernel/base/include \
        -I $(TOP_DIR)/kernel/extended/include
        C_INCLUDES += $(KERNEL_INC)

INCLUDE_INC = \
        -I $(TOP_DIR)/include
        C_INCLUDES += $(INCLUDE_INC)

CMSIS_INC = \
        -I $(TOP_DIR)/osdepends/liteos/cmsis
        C_INCLUDES += $(CMSIS_INC)

ARCH_INC = \
        -I $(TOP_DIR)/arch/arm/arm-m/include \
        -I $(TOP_DIR)/arch/arm/common/cmsis
        C_INCLUDES += $(ARCH_INC)

COMPONENTS_INC = \
        -I $(TOP_DIR)/components/osport \
        -I $(TOP_DIR)/components/at \
        -I $(TOP_DIR)/components/shell \
        -I $(TOP_DIR)/components/driver
        C_INCLUDES += $(COMPONENTS_INC)

DEVICEDRIV_INC = \
        -I $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Inc           \
        -I $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Inc/Legacy    \
        -I $(TOP_DIR)/targets/IoTClub-EVB-L1/DeviceDriver/NB                    \
        -I $(TOP_DIR)/targets/IoTClub-EVB-L1/DeviceDriver/BH1750                \
        -I $(TOP_DIR)/targets/IoTClub-EVB-L1/DeviceDriver/DHT11                 \
        -I $(TOP_DIR)/targets/IoTClub-EVB-L1/DeviceDriver/GPS                   \
        -I $(TOP_DIR)/targets/IoTClub-EVB-L1/Hardware                           \
        -I $(TOP_DIR)/targets/IoTClub-EVB-L1/Apps                               \
        -I $(TOP_DIR)/targets/IoTClub-EVB-L1/OS_CONFIG                          \
        -I $(TOP_DIR)/targets/IoTClub-EVB-L1/Usr

        C_INCLUDES += $(DEVICEDRIV_INC)

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"

# excluded unnecessary warnings
CFLAGS += -Wno-missing-braces


#######################################
# LDFLAGS
#######################################
# link script

LD_FILE = IoTClub-EVB-L1.ld
LDSCRIPT = $(PROJECTBASE)/$(LD_FILE)

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


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
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile  | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

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