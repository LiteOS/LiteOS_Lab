######################################
# target
######################################
LIB	= libkernel.a

######################################
# paths
######################################
CUR_DIR		= $(PWD)
TOP_DIR		= $(CUR_DIR)/../../../../..
BUILD_DIR	= objs
KERNEL_DIR	= $(TOP_DIR)/iot_link/os/os_imp/liteos
LLT_DIR		= $(TOP_DIR)/test/llt
DEST_DIR	= $(LLT_DIR)/mock/lib


######################################
# config
######################################
include $(LLT_DIR)/config

######################################
# binaries
######################################
CC	= $(PREFIX)gcc
CPP	= $(PREFIX)g++
LD	= $(PREFIX)ld
AR	= $(PREFIX)ar

######################################
# source
######################################
# C sources
C_SOURCES +=  \
        $(wildcard $(KERNEL_DIR)/los_init.c) \
        $(wildcard $(KERNEL_DIR)/base/core/*.c) \
        $(wildcard $(KERNEL_DIR)/base/ipc/*.c) \
        $(wildcard $(KERNEL_DIR)/base/mem/membox/*.c) \
        $(wildcard $(KERNEL_DIR)/base/mem/common/*.c) \
        $(wildcard $(KERNEL_DIR)/base/misc/*.c) \
        $(wildcard $(KERNEL_DIR)/base/om/*.c) \
        $(wildcard $(KERNEL_DIR)/extended/tickless/*.c) \
		$(wildcard $(LLT_DIR)/mock/src/*.c)

ifeq ($(CONFIG_MEM_TYPE), bestfit)
C_SOURCES += $(wildcard $(KERNEL_DIR)/base/mem/bestfit/*.c)
else ifeq ($(CONFIG_MEM_TYPE), tlsf)
C_SOURCES += $(wildcard $(KERNEL_DIR)/base/mem/tlsf/*.c)
else ifeq ($(CONFIG_MEM_TYPE), bestfit_little)
C_SOURCES += $(wildcard $(KERNEL_DIR)/base/mem/bestfit_little/*.c)
else
C_SOURCES += $(wildcard $(KERNEL_DIR)/base/mem/heap/*.c)
C_SOURCES += $(wildcard $(KERNEL_DIR)/utils/*.c)
endif

#######################################
# CFLAGS
#######################################
# C defines
C_DEFS = -D USE_LITEOS 
		
# C includes
C_INCLUDES = \
        -I $(KERNEL_DIR)/base/include  \
		-I $(KERNEL_DIR)/arch/arm/arm-m/include \
        -I $(KERNEL_DIR)/extended/include  \
        -I $(KERNEL_DIR)/include \
        -I $(KERNEL_DIR)/base/core \
		-I $(LLT_DIR)/mock/include

		
# compile gcc flags
CFLAGS += $(C_DEFS) $(C_INCLUDES)

#######################################
# build the application
#######################################
# objects
LIB_OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(patsubst %.c, %.o, $(C_SOURCES))))
vpath %.c $(sort $(dir $(C_SOURCES)))

all:$(BUILD_DIR) lib
lib:$(LIB)

# build lib	
$(LIB):$(LIB_OBJECTS)
#	$(CPP) -shared -o $@ $^
	$(AR) -r $@ $^
	mv $(LIB) $(DEST_DIR)

$(LIB_OBJECTS):$(BUILD_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR) $(DEST_DIR)
    
clean:
	rm -rf $(BUILD_DIR) *.a $(DEST_DIR)/$(LIB)
