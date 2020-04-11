################################################################################
# this is used for compile the iotlink
################################################################################

ifeq ($(CONFIG_LOG_ENABLE), y)

    log_src  = ${wildcard $(iot_link_root)/link_log/*.c} 
    C_SOURCES += $(log_src)	
    		
    log_inc = -I $(iot_link_root)/link_log
    C_INCLUDES += $(log_inc)
    
    C_DEFS += -D CONFIG_LINK_DEBUG_ENABLE=1

endif





