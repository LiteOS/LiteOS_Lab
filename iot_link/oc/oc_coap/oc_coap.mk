################################################################################
# this is used for compile the OC_COAP
################################################################################

ifeq ($(CONFIG_OC_COAP_ENABLE), y)

    OC_COAP_AL_SRC  = ${wildcard $(iot_link_root)/oc/oc_coap/oc_coap_al/*.c}
    C_SOURCES += $(OC_COAP_AL_SRC)	
    
    OC_COAP_AL_INC = -I $(iot_link_root)/oc/oc_coap/oc_coap_al
    C_INCLUDES += $(OC_COAP_AL_INC)	
    
    oc_coap_defs = -D CONFIG_OC_COAP_ENABLE=1
    C_DEFS += $(oc_coap_defs)
    
    ifeq ($(CONFIG_OC_COAP_TYPE), "soft")    
		include $(iot_link_root)/oc/oc_coap/atiny_coap/atiny_coap.mk
    endif

endif