################################################################################
# STM32L431_BearPI GCC compiler Makefile
################################################################################
# ------------------------------------------------
# Generic Makefile (based on gcc)
# ------------------------------------------------

################################################################################
# target
################################################################################
TARGET = Huawei_LiteOS
################################################################################
# building variables
################################################################################
# debug build?
DEBUG = 1
# optimization
OPT = -O0 -g

################################################################################
# binaries
################################################################################
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


################################################################################
# paths
################################################################################
BUILD_DIR = build
LD_FILE = os.ld
MAKEFILE_DIR = $(abspath $(CURDIR))
LDSCRIPT = $(MAKEFILE_DIR)/$(LD_FILE)
TARGET_DIR = $(abspath $(MAKEFILE_DIR)/..)

#we should export the SDK_DIR FOR THE iot.mk and project.mk use
ifndef SDK_DIR
    SDK_DIR :=$(abspath $(MAKEFILE_DIR)/../../..)
endif

DRIVERLIB_DIR := $(SDK_DIR)/drivers
IOTLINK_DIR := $(SDK_DIR)/iot_link

################################################################################
#common variables for other module or components
C_SOURCES =
C_DEFS =
C_INCLUDES =
LDFLAGS =
ASM_SOURCES_S =
AS_DEFS =
AS_INCLUDES =
PERIFLIB_SOURCES =
LIBS =
LIBDIR =
LDFLAGS =
ASFLAGS =
CFLAGS =

##########################LOAD THE SOURCES INCLUDES AND DEFINES#################
include ../.config
include $(SDK_DIR)/iot_link/iot.mk
include $(MAKEFILE_DIR)/project.mk


################################################################################
# firmware library
################################################################################


################################################################################
# CFLAGS
################################################################################
# cpu
CPU = -mcpu=cortex-m4
# fpu
FPU = -mfpu=fpv4-sp-d16
# float-abi
FLOAT-ABI = -mfloat-abi=hard
# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# compile gcc flags
ASFLAGS += $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$@"

# excluded unnecessary warnings
CFLAGS += -Wno-missing-braces


# libraries
LIBS += -lc -lm -lnosys
LIBDIR  += -L$(TARGET_DIR)/Lib
LDFLAGS += $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
LDFLAGS += -u _printf_float


############HERE WE GET THE C_OBJECT S_OBJECT AND THE PATH #####################
#all the code for the compile:
#iot_link: which indexed by the iot_link_root, it is the sdk main 
#drivers: which maybe used by the targets, it is common for the MCUS
#targets: which developed by the user

#changge the path for all the sources in directory iot_link  targets drivers
#WE WILL CREATE SAME RELATION SHIP IN THE BUILD_DIR, AND IF YOU HAVE SOURCES 
#out of SDK_DIR TARGET_DIR, please add them as THIS
C_OBJ :=
C_OBJ += $(patsubst $(IOTLINK_DIR)/%.c,$(BUILD_DIR)/iot_link/%.o,$(filter $(IOTLINK_DIR)/%.c,$(C_SOURCES)))
C_OBJ += $(patsubst $(DRIVERLIB_DIR)/%.c,$(BUILD_DIR)/drivers/%.o,$(filter $(DRIVERLIB_DIR)/%.c,$(C_SOURCES)))
C_OBJ += $(patsubst $(TARGET_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TARGET_DIR)/%.c,$(C_SOURCES)))

S_OBJ :=
S_OBJ += $(patsubst $(IOTLINK_DIR)/%.S,$(BUILD_DIR)/iot_link/%.o,$(filter $(IOTLINK_DIR)/%.S,$(ASM_SOURCES_S)))
S_OBJ += $(patsubst $(DRIVERLIB_DIR)/%.S,$(BUILD_DIR)/drivers/%.o,$(filter $(DRIVERLIB_DIR)/%.S,$(ASM_SOURCES_S)))
S_OBJ += $(patsubst $(TARGET_DIR)/%.S,$(BUILD_DIR)/%.o,$(filter $(TARGET_DIR)/%.S,$(ASM_SOURCES_S)))

OBJ_DIRS :=
OBJ_DIRS +=  $(dir $(C_OBJ))
OBJ_DIRS +=  $(dir $(S_OBJ))
OBJ_DIRS := $(sort $(OBJ_DIRS))

#ADD THE SDK_DIR AND TARGET_DIR TO THE SEARCH PATH
vpath %.c $(SDK_DIR):$(TARGET_DIR)
vpath %.S $(SDK_DIR):$(TARGET_DIR)

#NOW DO THE BUILDING
all:$(BUILD_DIR)/$(TARGET).elf
	-$(HEX) $< $(BUILD_DIR)/$(TARGET).hex
	-$(BIN) $< $(BUILD_DIR)/$(TARGET).bin	

$(BUILD_DIR)/$(TARGET).elf:$(OBJ_DIRS) $(C_OBJ) $(S_OBJ)
	$(CC) $(C_OBJ) $(S_OBJ) $(LDFLAGS) -o $@
	$(SZ) $@
		
#create the necessary path for the object
$(OBJ_DIRS):
	-mkdir -p $@

#compile the c file to the object, as you know we has map the .c to the .o	
$(C_OBJ):$(BUILD_DIR)/%.o:%.c
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(@:%.o=%.lst) $<  -o $@

#compile the s fike to the object, as you know we has map the .S to the .o
$(S_OBJ):$(BUILD_DIR)/%.o:%.S
	$(AS) -c $(CFLAGS) $< -o $@


################################################################################
# clean up: all you need to do is to remove the build dirs
################################################################################
clean:
	-rm -fR .dep $(BUILD_DIR)

# *** EOF ***
