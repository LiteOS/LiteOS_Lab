################################################################################
# this is used for compile the iotlink
################################################################################

LINK_MISC_SOURCE  = ${wildcard $(TOP_DIR)/iot_link/link_misc/*.c} 
C_SOURCES += $(LINK_MISC_SOURCE)	
		
LINK_MISC_INC = -I $(TOP_DIR)/iot_link/link_misc
C_INCLUDES += $(LINK_MISC_INC)


