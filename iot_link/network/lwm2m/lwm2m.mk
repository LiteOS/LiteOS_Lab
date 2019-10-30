################################################################################
# this is used for compile the lwm2m
################################################################################
ifeq ($(CONFIG_LWM2M_ENABLE), y)

    LWM2M_MODULE_SOURCE  = ${wildcard $(iot_link_root)/network/lwm2m/lwm2m_al/*.c}
    C_SOURCES += $(LWM2M_MODULE_SOURCE)
    		
    LWM2M_MODULE_INC = -I $(iot_link_root)/network/lwm2m/lwm2m_al
    C_INCLUDES += $(LWM2M_MODULE_INC)
    
 	ifeq ($(CONFIG_LWM2M_TYPE),"wakaama_lwm2m")
    	include $(iot_link_root)/network/lwm2m/wakaama_lwm2m/wakaama.mk
 	else ifeq ($(CONFIG_LWM2M_TYPE),"wakaama_raw")
    	include $(iot_link_root)/network/lwm2m/wakaama_raw/wakaama.mk
    else
    	#you could extend the lwm2m support implement
	endif
endif
