################################################################################
# this is used for compile the user ocmqtt application
################################################################################
#example for cloud map 
ifeq ($(CONFIG_OCMQTT_APP), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Demos/ocmqtt_app/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Demos/ocmqtt_app
	C_DEFS += -D CONFIG_OCMQTT_APP=1
endif

