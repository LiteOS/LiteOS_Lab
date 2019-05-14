################################################################################
# this is used for compile the hilink
################################################################################
ifeq ($(WITH_HILINK), yes)
HILINK_SRC =  \
        $(TOP_DIR)/components/hilink_hislip/hilinkexample.c \
        $(TOP_DIR)/components/hilink_hislip/src/dev_profile.c \
        $(TOP_DIR)/components/hilink_hislip/src/hilink.c \
        $(TOP_DIR)/components/hilink_hislip/src/hislip.c \
        $(TOP_DIR)/components/hilink_hislip/src/hislip_uart.c \
        $(TOP_DIR)/components/hilink_hislip/port/hislip_port.c \
        $(TOP_DIR)/components/hilink_hislip/port/bsp_led.c
        C_SOURCES += $(HILINK_SRC)
endif
ifeq ($(WITH_HILINK), yes)
HILINK_INC = \
        -I $(TOP_DIR)/components/hilink_hislip/inc \
        -I $(TOP_DIR)/components/hilink_hislip/profile
        C_INCLUDES += $(HILINK_INC)
endif