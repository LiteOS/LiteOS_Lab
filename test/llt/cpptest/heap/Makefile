CUR_DIR   := $(abspath $(CURDIR))
TOP_DIR   := $(abspath $(CUR_DIR)/../../../..)
SDK_DIR   := $(TOP_DIR)
TEST_DIR  := $(TOP_DIR)/test
LLT_DIR   := $(TEST_DIR)/llt/cpptest

######################################
# config
######################################
# match files that display the coverage rate
FILE_PATTERN = "*los_heap.c" "*los_memory.c"

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
        $(wildcard $(TOP_DIR)/iot_link/os/liteos/base/mem/common/*.c) \
        $(wildcard $(TOP_DIR)/iot_link/os/liteos/base/mem/heap/*.c) \
        $(wildcard $(TOP_DIR)/iot_link/os/liteos/base/mem/membox/*.c) \
        $(wildcard $(TOP_DIR)/iot_link/os/liteos/utils/*.c)


#######################################
# CFLAGS
#######################################
# C++ includes
CPP_INCLUDES  += -I $(CUR_DIR)

# C includes
C_INCLUDES    += \
        -I $(CUR_DIR)/adapter \
        -I $(TOP_DIR)/iot_link/os/liteos/include \
        -I $(TOP_DIR)/iot_link/os/liteos/base/include

#######################################
# build the application
#######################################
include $(LLT_DIR)/build.mk
