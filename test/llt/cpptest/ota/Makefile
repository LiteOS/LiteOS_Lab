CUR_DIR   := $(abspath $(CURDIR))
TOP_DIR   := $(abspath $(CUR_DIR)/../../../..)
SDK_DIR   := $(TOP_DIR)
TEST_DIR  := $(TOP_DIR)/test
LLT_DIR   := $(TEST_DIR)/llt/cpptest

######################################
# config
######################################
# match files that display the coverage rate
FILE_PATTERN = "*ota_flag.c" "*pcp.c" "*ota_pack.c" "*ota_checksum.c"

# common config
include $(LLT_DIR)/config.mk

# private config
include $(CUR_DIR)/config.mk

######################################
# source
######################################
# C++ sources
CPP_SOURCES += \
		$(wildcard $(CUR_DIR)/*.cpp) \
		$(wildcard $(CUR_DIR)/pcp/*.cpp) \
		$(wildcard $(CUR_DIR)/manager/*.cpp)

# C sources
C_SOURCES   += \
		$(wildcard $(CUR_DIR)/*.c) \
		$(wildcard $(CUR_DIR)/adapter/*.c)

#######################################
# CFLAGS
#######################################
# C++ includes
CPP_INCLUDES  += \
		-I $(CUR_DIR) \
		-I $(CUR_DIR)/pcp \
		-I $(CUR_DIR)/manager

# C includes
C_INCLUDES    += \
		-I $(CUR_DIR)/adapter

#######################################
# build the application
#######################################
include $(LLT_DIR)/build.mk
