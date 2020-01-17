CUR_DIR   := $(abspath $(CURDIR))
TOP_DIR   := $(abspath $(CUR_DIR)/../../../..)
SDK_DIR   := $(TOP_DIR)

all:
	@$(shell mv $(TOP_DIR)/iot_link/inc/shell.h $(TOP_DIR)/iot_link/inc/shell.h.bak)
	make -f __Makefile
	@$(shell mv $(TOP_DIR)/iot_link/inc/shell.h.bak $(TOP_DIR)/iot_link/inc/shell.h)

clean:
	make -f __Makefile clean

run:
	make -f __Makefile run

cov:
	make -f __Makefile cov

memcheck:
	make -f __Makefile memcheck