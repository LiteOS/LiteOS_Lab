##########################################################################################################################
# STM32F429IGTx_FIRE GCC compiler Makefile
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
# ------------------------------------------------

ifndef CONFIG_FILE
    CONFIG_FILE = config.mk
endif

include $(CONFIG_FILE)
include prune.mk
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

USE_OTA := no
OTA_IS_NEED_DTLS := no



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
#common variables for other module or components
C_SOURCES =
C_DEFS =
C_INCLUDES =
LDFLAGS =
ASM_SOURCES_S =
######################################

###############LOAD IOT_LINK###############
IOT_LINK = yes
#load iot link
ifeq ($(IOT_LINK), yes)
include $(TOP_DIR)/iot_link/iot_link.mk
include project.mk
endif

######################################
# C sources


       
ifeq ($(USE_BOOTLOADER), no)

SOCKET_ADAPTER_SRC = \
        ${wildcard $(TOP_DIR)/components/net/sal/*.c}
        C_SOURCES += $(SOCKET_ADAPTER_SRC)

NEWLIB_SRC = \
        ${wildcard $(TOP_DIR)/components/lib/libc/malloc.c}\
        ${wildcard $(TOP_DIR)/components/lib/libc/newlib_stub.c} \
        ${wildcard $(TOP_DIR)/components/lib/libc/errno.c}
        C_SOURCES += $(NEWLIB_SRC)


ATINY_LOG = \
        ${wildcard $(TOP_DIR)/components/log/*.c}
        C_SOURCES += $(ATINY_LOG)







ifeq ($(NETWORK_TYPE), NB_NEUL95_NO_ATINY)
    NB_NEUL95_ONYL_DEMO = ${wildcard $(TOP_DIR)/demos/nbiot_without_atiny/nb_demo.c}
    C_SOURCES += $(NB_NEUL95_ONYL_DEMO)
endif

ifeq ($(USE_SOTA), yes)
SOTA_SRC = \
        ${wildcard $(TOP_DIR)/components/ota/sota/*.c}
        C_SOURCES += $(SOTA_SRC)
        SOTA_DEMO = ${wildcard $(TOP_DIR)/demos/nbiot_without_atiny/sota_demo.c}
        C_SOURCES += $(SOTA_DEMO)
endif

ifeq ($(USE_OTA), yes)
OTA_SRC = \
    ${wildcard  $(TOP_DIR)/components/ota/flag_operate/*.c} \
    ${wildcard  $(TOP_DIR)/components/ota/package/*.c} \
    ${wildcard  $(TOP_DIR)/components/ota/utility/*.c}
    C_SOURCES += $(OTA_SRC)
C_SOURCES += $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/board.c
C_SOURCES += $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/ota_port.c
#MQTT has add flash_adaptor.c 
ifneq ($(WITH_MQTT), yes)
C_SOURCES += $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/flash_adaptor.c
endif
endif



#blow use bootloader
else 

FLAG_SRC = \
        ${wildcard $(TOP_DIR)/components/ota/flag_operate/*.c}
        C_SOURCES += $(FLAG_SRC)

USER_SRC = \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/board.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/bootloader_recover.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/system_stm32f4xx.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/usart.c
        C_SOURCES += $(USER_SRC)

endif

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
C_DEFS +=  \
    -D USE_HAL_DRIVER \
    -D STM32F429xx \
    -D NDEBUG

ifeq ($(WITH_HILINK), yes)
    C_DEFS += -DWITH_HILINK
endif

ifeq ($(WITH_LWM2M), yes)
C_DEFS += \
    -D LWM2M_LITTLE_ENDIAN \
    -D LWM2M_CLIENT_MODE
endif


ifeq ($(WITH_AT_FRAMEWORK), yes)
    C_DEFS += -DWITH_AT_FRAMEWORK
    C_DEFS += -DUSE_$(NETWORK_TYPE)
endif


ifeq ($(OTA_PACK_CHECKSUM), NO_CHECKSUM)
    C_DEFS += -DPACK_CHECKSUM=2
else ifeq ($(OTA_PACK_CHECKSUM), SHA256)
    C_DEFS += -DPACK_CHECKSUM=1
else ifeq ($(OTA_PACK_CHECKSUM), SHA256_RSA2048)
    C_DEFS += -DPACK_CHECKSUM=0
else
endif



ifeq ($(USE_FOTA), yes)
    C_DEFS += -DCONFIG_FEATURE_FOTA
endif

ifeq ($(USE_SOTA), yes)
    C_DEFS += -DWITH_SOTA
endif

ifeq ($(LWM2M_BOOTSTRAP), yes)
    C_DEFS += -DLWM2M_BOOTSTRAP
endif

ifeq ($(SUPPORT_DTLS_SRV), yes)
    C_DEFS += -DSUPPORT_DTLS_SRV
endif

ifeq ($(LWM2M_WITH_LOGS), yes)
    C_DEFS += -DLWM2M_WITH_LOGS
endif

ifeq ($(ATINY_DEBUG), yes)
    C_DEFS += -DATINY_DEBUG
endif

ifeq ($(WITH_FILESYSTEM), yes)
    C_DEFS += -DFS_$(FILESYSTEM_TYPE)
endif





# AS includes
AS_INCLUDES =

# C includes
HAL_DRIVER_INC = \
        -I $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Inc \
        -I $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Inc/Legacy
        C_INCLUDES += $(HAL_DRIVER_INC)

INCLUDE_INC = \
        -I $(TOP_DIR)/include
        C_INCLUDES += $(INCLUDE_INC)

LWM2M_ATINY_INC = \
        -I $(TOP_DIR)/components/connectivity/agent_tiny/atiny_lwm2m        
        C_INCLUDES += $(LWM2M_ATINY_INC)

HARDWARE_INC = \
        -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Hardware/Inc
        C_INCLUDES += $(HARDWARE_INC)



USER_INC = \
        -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Inc
        C_INCLUDES += $(USER_INC)

ifneq ($(USE_OTA)_$(USE_BOOTLOADER), no_no)
OTA_INC = \
        -I $(TOP_DIR)/components/ota/flag_operate \
        -I $(TOP_DIR)/components/ota/utility
        C_INCLUDES += $(OTA_INC)
endif


OS_CONFIG_INC = \
        -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE/OS_CONFIG
        C_INCLUDES += $(OS_CONFIG_INC)

        
ifeq ($(USE_BOOTLOADER), no)


ifeq ($(WITH_LWM2M), yes)
ER_COAP_INC = \
        -I $(TOP_DIR)/components/connectivity/lwm2m/core/er-coap-13
        C_INCLUDES += $(ER_COAP_INC)
LWM2M_INC = \
        -I $(TOP_DIR)/components/connectivity/lwm2m/core \
        -I $(TOP_DIR)/components/connectivity/lwm2m/examples/shared
        C_INCLUDES += $(LWM2M_INC)
endif

DEMO_INC = \
        -I $(TOP_DIR)/demos
        C_INCLUDES += $(DEMO_INC)

ifeq ($(WITH_FILESYSTEM), yes)
FS_SPIFFS_INC = \
        -I $(TOP_DIR)/components/fs/spiffs/spiffs_git/src \
        -I $(TOP_DIR)/components/fs/spiffs/spiffs_git/src/default

FS_FATFS_INC = \
        -I $(TOP_DIR)/components/fs/fatfs/ff13b/source \
        -I $(TOP_DIR)/components/fs/fatfs/ff13b/source/default

ifeq ($(IS_COMPILE_ALLFS), ALL)
        C_INCLUDES += $(FS_SPIFFS_INC)
        C_INCLUDES += $(FS_FATFS_INC)
else ifeq ($(FILESYSTEM_TYPE), SPIFFS)
        C_INCLUDES += $(FS_SPIFFS_INC)
else ifeq ($(FILESYSTEM_TYPE), FATFS)
        C_INCLUDES += $(FS_FATFS_INC)
endif
endif

ifeq ($(WITH_CMOCKERY_TEST), yes)
CMOCKERY_TEST_INC = \
        -I $(TOP_DIR)/tests/cmockery/src/google
        C_INCLUDES += $(CMOCKERY_TEST_INC)
endif

#below use bootloader
else
FLAG_INC = \
        -I $(TOP_DIR)/components/ota/utility \
        -I $(TOP_DIR)/components/ota/flag_operate
        C_INCLUDES += $(FLAG_INC)
endif



# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$@"

# excluded unnecessary warnings
CFLAGS += -Wno-missing-braces


#######################################
# LDFLAGS
#######################################
# link script
ifeq ($(USE_BOOTLOADER), yes)
LD_FILE = STM32F429IGTx_LiteOS.ld
else ifeq ($(USE_OTA), yes)
LD_FILE = STM32F429IGTx_LiteOS_bootloader.ld
else
LD_FILE = STM32F429IGTx_LiteOS.ld
endif

LDSCRIPT = $(PROJECTBASE)/$(LD_FILE)

# libraries
LIBS = -lc -lm -lnosys
ifeq ($(USE_OTA), yes)
LIBS += -lhwpatch
endif
LIBDIR = -L$(PROJECTBASE)/../Lib
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

ifeq ($(WITH_MQTT), yes)
LDFLAGS += -u _printf_float
endif

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES_S:.S=.o)))
vpath %.S $(sort $(dir $(ASM_SOURCES_S)))

$(BUILD_DIR)/%.o: %.c Makefile config.mk prune.mk | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile config.mk prune.mk | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile config.mk prune.mk
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
