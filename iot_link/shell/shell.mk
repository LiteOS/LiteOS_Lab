################################################################################
# this is used for compile the iotlink
################################################################################

SHELL_SOURCE  = ${wildcard $(TOP_DIR)/iot_link/shell/*.c} 
C_SOURCES += $(SHELL_SOURCE)	
		
SHELL_INC = -I $(TOP_DIR)/iot_link/shell
C_INCLUDES += $(SHELL_INC)

C_DEFS += -DLINK_CFG_EN_SHELL



