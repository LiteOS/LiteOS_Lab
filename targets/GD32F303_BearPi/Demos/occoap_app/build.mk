################################################################################
# this is used for compile the user ocmqtt application
################################################################################
#example for cloud map 
ifeq ($(CONFIG_OCCOAP_APP), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Demos/occoap_app/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Demos/occoap_app
	C_DEFS += -D CONFIG_OCCOAP_APP=1
endif

