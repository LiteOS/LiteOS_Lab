################################################################################
# this is used for compile the ota function
################################################################################

#we need the crc
ifeq ($(CONFIG_OTA_ENABLE),y)

    OTA_MODULE_SRC  = ${wildcard $(iot_link_root)/ota/*.c} \
			${wildcard $(iot_link_root)/ota/manager/*.c}
    C_SOURCES += $(OTA_MODULE_SRC)	
    		
    OTA_MODULE_INC = -I $(iot_link_root)/ota -I $(iot_link_root)/ota/manager
    C_INCLUDES += $(OTA_MODULE_INC)
    
    C_DEFS += -D CONFIG_OTA_ENABLE=1
    
    ifeq ($(CONFIG_PCP_ENABLE), y)
		include $(iot_link_root)/ota/pcp/pcp.mk
	endif
		
endif	
	



