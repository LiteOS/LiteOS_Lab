CUR_DIR   := $(abspath $(CURDIR))
TOP_DIR   := $(abspath $(CUR_DIR)/../../../..)
SDK_DIR   := $(TOP_DIR)
TEST_DIR  := $(TOP_DIR)/test
LLT_DIR   := $(TEST_DIR)/llt/cpptest

######################################
# config
######################################
# match files that display the coverage rate
FILE_PATTERN = "*los_tickless.c"

# common config
include $(LLT_DIR)/config.mk

# private config
include $(CUR_DIR)/config.mk

######################################
# source
######################################
# C++ sources
CPP_SOURCES += $(wildcard $(CUR_DIR)/*.cpp)

# C sources
C_SOURCES   += \
        $(wildcard $(CUR_DIR)/*.c) \
        $(wildcard $(CUR_DIR)/adapter/*.c) \
        $(wildcard $(TOP_DIR)/iot_link/os/liteos/extended/tickless/*.c)


#######################################
# CFLAGS
#######################################
# C++ includes
CPP_INCLUDES  += -I $(CUR_DIR)

# C includes
C_INCLUDES    += \
        -I $(CUR_DIR)/adapter \
        -I $(TOP_DIR)/iot_link/os/liteos/include \
        -I $(TOP_DIR)/iot_link/os/liteos/base/include \
        -I $(TOP_DIR)/iot_link/os/liteos/extended/include

C_DEFS += -DLOSCFG_KERNEL_TICKLESS=1 -DLOSCFG_PLATFORM_HWI=0

#######################################
# build the application
#######################################
include $(LLT_DIR)/build.mk
