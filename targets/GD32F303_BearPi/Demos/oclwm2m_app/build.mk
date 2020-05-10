################################################################################
# this is used for compile the user ocmqtt application
################################################################################
#example for cloud map 
ifeq ($(CONFIG_OCLWM2M_APP), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Demos/oclwm2m_app/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Demos/oclwm2m_app
	C_DEFS += -D CONFIG_OCLWM2M_APP=1
endif

