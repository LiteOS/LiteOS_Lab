#
# Copyright (c) <2020>, <Huawei Technologies Co., Ltd>. All rights reserved
#

######################################
# paths
######################################
BUILD_DIR = build

######################################
# target
######################################
BIN := test
LIB := $(BUILD_DIR)/libsdk.a

######################################
# binaries
######################################
CC	= $(PREFIX)gcc
CPP	= $(PREFIX)g++
LD	= $(PREFIX)ld
AR	= $(PREFIX)ar

#######################################
# FLAGS
#######################################
CFLAGS    += -fdata-sections -ffunction-sections
CFLAGS    += $(C_DEFS) $(C_INCLUDES) $(CPP_INCLUDES)
CPP_FLAGS += -std=c++11 -fpermissive

# LIBS      += -lsdk -lcpptest -lpthread -lrt
LIBS      += -lcpptest -lpthread -lrt
LIB_DIR   += -L$(BUILD_DIR)
LDFLAGS   += $(LIB_DIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(BIN).map

#######################################
# build
#######################################
C_OBJECTS   := $(addprefix $(BUILD_DIR)/, $(notdir $(patsubst %.c, %.o, $(C_SOURCES))))
vpath %.c $(sort $(dir $(C_SOURCES)))

CPP_OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(patsubst %.cpp, %.o, $(CPP_SOURCES))))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

OBJECTS := $(CPP_OBJECTS) $(C_OBJECTS)

# start
# all:$(BUILD_DIR) $(LIB) $(BIN)
all:$(BUILD_DIR) $(BIN)
	@echo "OK"

$(BIN):$(OBJECTS)
	@$(CPP) $(CFLAGS) $(CPP_FLAGS) -o $@ $^ $(LDFLAGS)

$(CPP_OBJECTS):$(BUILD_DIR)/%.o:%.cpp
	@$(CPP) $(CFLAGS) $(CPP_FLAGS) -c $< -o $@

$(LIB):$(C_OBJECTS)
#	$(CPP) -shared -o $@ $^
	@$(AR) -r $@ $^

$(C_OBJECTS):$(BUILD_DIR)/%.o:%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN) gcov *.htm *.log

run:
	./$(BIN)

cov:run
	@mkdir -p gcov
	lcov -c -o gcov/gcov.info -d . -b . --rc lcov_branch_coverage=1
	lcov -e gcov/gcov.info $(FILE_PATTERN) -o gcov/gcov.info --rc lcov_branch_coverage=1
	genhtml -o gcov/html gcov/gcov.info  --rc genhtml_branch_coverage=1

memcheck:
	valgrind $(VALGRIND_FLAGS) ./$(BIN) 2>&1 | tee memcheck.log
