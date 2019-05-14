################################################################################
# this is used for compile the iotlink
################################################################################

SHELL_SOURCE  = ${wildcard $(iot_link_root)/shell/*.c} 
C_SOURCES += $(SHELL_SOURCE)	
		
SHELL_INC = -I $(iot_link_root)/shell
C_INCLUDES += $(SHELL_INC)

C_DEFS += -D CFG_SHELL_ENABLE=1



