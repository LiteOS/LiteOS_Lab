######################################
# target
######################################
BIN = test

######################################
# paths
######################################
CUR_DIR		= $(PWD)
TOP_DIR		= $(CUR_DIR)/../../..
BUILD_DIR	= objs
TEST_DIR	= $(TOP_DIR)/test
GTEST_DIR	= $(TEST_DIR)/3rd/googletest
LLT_DIR		= $(TEST_DIR)/llt
MOCK_DIR	= $(LLT_DIR)/mock
GCOV_DIR	= $(MOCK_DIR)/build/kernel
KERNEL_DIR	= $(TOP_DIR)/iot_link/os/os_imp/liteos

######################################
# config
######################################
FILE_PATTERN = "*mem*"

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

# C++ sources
CPP_SOURCES = $(wildcard $(CUR_DIR)/*.cpp)

CPP_SOURCES += $(wildcard $(CUR_DIR)/mem/*.cpp)

#######################################
# CFLAGS
#######################################
# C defines
C_DEFS = -DUSE_LITEOS
		
# C includes
C_INCLUDES = \
		-I $(KERNEL_DIR)/arch/arm/arm-m/include \
        -I $(KERNEL_DIR)/base/include  \
        -I $(KERNEL_DIR)/extended/include  \
        -I $(KERNEL_DIR)/include \
        -I $(KERNEL_DIR)/base/core \
		-I $(MOCK_DIR)/include \
		-I $(GTEST_DIR)/include

# C++ includes
CPP_INCLUDES = -I .
		
# compile gcc flags
CFLAGS += $(C_DEFS) $(C_INCLUDES) $(CPP_INCLUDES)

# library
LIB_DIR += -L$(CUR_DIR) -L$(MOCK_DIR)/lib -L$(GTEST_DIR)/lib
LIBS += -lkernel -lgtest -lpthread
LDFLAGS += $(LIB_DIR) $(LIBS)

#######################################
# build the application
#######################################
# objects
CPP_OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(patsubst %.cpp, %.o, $(CPP_SOURCES))))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

all:$(BUILD_DIR) $(BIN)

# build bin
$(BIN):$(CPP_OBJECTS)
	$(CPP) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CPP_OBJECTS):$(BUILD_DIR)/%.o:%.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

distclean:clean
	# make -C $(GCOV_DIR) clean
	@cd $(GCOV_DIR); make clean; cd -
	
clean:
	rm -rf $(BUILD_DIR) $(BIN) gcov *.a *.htm *.log

run:
	./$(BIN)

cov:run
	@mkdir -p gcov
	lcov -c -o gcov/gcov.info -d $(GCOV_DIR) -b $(GCOV_DIR) --rc lcov_branch_coverage=1
	lcov -e gcov/gcov.info $(FILE_PATTERN) -o gcov/gcov.info --rc lcov_branch_coverage=1
	genhtml -o gcov/html gcov/gcov.info --rc genhtml_branch_coverage=1

memcheck:
	valgrind $(VALGRIND_FLAGS) ./$(BIN) 2>&1 | tee memcheck.log
