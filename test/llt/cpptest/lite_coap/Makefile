CUR_DIR   := $(abspath $(CURDIR))
TOP_DIR   := $(abspath $(CUR_DIR)/../../../..)
SDK_DIR   := $(TOP_DIR)
TEST_DIR  := $(TOP_DIR)/test
LLT_DIR   := $(TEST_DIR)/llt/cpptest

######################################
# config
######################################
# match files that display the coverage rate
FILE_PATTERN = "*coap_core.c" "*coap_osdepends.c" "*litecoap_port.c" "*coap_al.c"

# common config
include $(LLT_DIR)/config.mk

# private config
include $(CUR_DIR)/config.mk

######################################
# source
######################################
# C++ sources
CPP_SOURCES += $(wildcard $(CUR_DIR)/*.cpp)
CPP_OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(patsubst %.cpp, %.o, $(CPP_SOURCES))))

# C sources
C_SOURCES   += $(wildcard $(CUR_DIR)/*.c)
C_OBJ:=$(addprefix $(BUILD_DIR)/, $(notdir $(patsubst %.c, %.o, $(C_SOURCES))))

#######################################
# CFLAGS
#######################################
# C++ includes
CPP_INCLUDES  += -I $(CUR_DIR)

# C includes
C_INCLUDES    +=

#######################################
# build the application
#######################################
include $(LLT_DIR)/build.mk
