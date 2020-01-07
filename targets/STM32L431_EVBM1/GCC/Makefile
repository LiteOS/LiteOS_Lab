################################################################################
# STM32L431 GCC compiler Makefile
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

USE_OTA := no
OTA_IS_NEED_DTLS := no

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


PROJECTBASE = $(abspath $(CURDIR))
TOP_DIR     = $(abspath $(PROJECTBASE)/../../..)

#this is for IoT Studio automatic generating kconfig compatibility
ifndef SDK_DIR
    SDK_DIR=$(abspath $(PROJECTBASE)/../../..)
endif


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
include $(PROJECTBASE)/config.mk
include $(PROJECTBASE)/project.mk
################################################################################
# paths
################################################################################

OBJ_DIRS :=
TARGET_DIR:= $(PROJECTBASE)/build

################################################################################
# LDFLAGS
################################################################################
LD_FILE = os.ld
LDSCRIPT = $(PROJECTBASE)/$(LD_FILE)

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
ifeq ($(USE_OTA), yes)
LIBS += -lhwpatch
endif
LIBDIR  += -L$(PROJECTBASE)/../Lib

LDFLAGS += $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(TARGET_DIR)/$(TARGET).map,--cref -Wl,--gc-sections



############HERE WE GET THE C_OBJECT S_OBJECT AND THE PATH #####################

C_OBJ := $(C_SOURCES:$(TOP_DIR)/%.c=$(TARGET_DIR)/%.o)
OBJ_DIRS +=  $(dir $(C_OBJ))
S_OBJ := $(ASM_SOURCES_S:$(TOP_DIR)/%.S=$(TARGET_DIR)/%.o)
OBJ_DIRS +=  $(dir $(S_OBJ))
OBJ_DIRS := $(sort $(OBJ_DIRS))

##################DO THE BUILDING NOW###########################################
# default action: build all
all:$(TARGET_DIR)/$(TARGET).elf
	-$(HEX) $< $(TARGET_DIR)/$(TARGET).hex
	-$(BIN) $< $(TARGET_DIR)/$(TARGET).hex	

$(TARGET_DIR)/$(TARGET).elf:$(OBJ_DIRS) $(C_OBJ) $(S_OBJ)
	$(CC) $(C_OBJ) $(S_OBJ) $(LDFLAGS) -o $@
	$(SZ) $@
		
#create the necessary path for the object
$(OBJ_DIRS):
	-mkdir -p $@

#compile the c file to the object, as you know we has map the .c to the .o	
$(C_OBJ):$(TARGET_DIR)/%.o:$(TOP_DIR)/%.c
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(@:%.o=%.lst) $<  -o $@

#compile the s fike to the object, as you know we has map the .S to the .o
$(S_OBJ):$(TARGET_DIR)/%.o:$(TOP_DIR)/%.S
	$(AS) -c $(CFLAGS) $< -o $@
		
 
################################################################################
# clean up
################################################################################
clean:
	-rm -fR .dep $(TARGET_DIR)


# *** EOF ***
