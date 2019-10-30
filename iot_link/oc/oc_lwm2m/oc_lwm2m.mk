################################################################################
# this is used for compile the oc lwm2m agent
################################################################################

ifeq ($(CONFIG_OC_LWM2M_ENABLE), y)

    OC_LWM2M_AL_SOURCE  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_al/*.c} 
    C_SOURCES += $(OC_LWM2M_AL_SOURCE)	
    		
    OC_LWM2M_AL_INC = -I $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_al
    C_INCLUDES += $(OC_LWM2M_AL_INC)
    
    OC_LWM2M_AL_DEFS = -D CONFIG_OC_LWM2M_ENABLE=1
    C_DEFS += $(OC_LWM2M_AL_DEFS)
    
    
    ifeq ($(CONFIG_OC_LWM2M_TYPE),"soft")
    	include $(iot_link_root)/oc/oc_lwm2m/atiny_lwm2m/atiny_lwm2m.mk
    else ifeq ($(CONFIG_OC_LWM2M_TYPE),"atiny_lwm2m_raw")
    	include $(iot_link_root)/oc/oc_lwm2m/atiny_lwm2m_raw/atiny_lwm2m.mk
    else ifeq ($(CONFIG_OC_LWM2M_TYPE),"boudica150_oc")
    	include $(iot_link_root)/oc/oc_lwm2m/boudica150_oc/boudica150_oc.mk
    else
            
    endif
    
endif