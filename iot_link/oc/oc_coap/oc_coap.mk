################################################################################
# this is used for compile the OC_COAP
################################################################################

ifeq ($(CONFIG_OCCOAP_ENABLE), y)

    C_SOURCES += $(iot_link_root)/oc/oc_coap/oc_coap_al/oc_coap_al.c    
    C_INCLUDES += -I $(iot_link_root)/oc/oc_coap/oc_coap_al
        
    ifeq ($(CONFIG_OCCOAP_DEMO_DTLS_ENABLE),y)
        C_SOURCES += $(iot_link_root)/oc/oc_coap/oc_coap_al/oc_dtls_coap_demo.c
    else ifeq ($(CONFIG_OCCOAP_DEMO_ENABLE),y)
        C_SOURCES += $(iot_link_root)/oc/oc_coap/oc_coap_al/oc_coap_demo.c              
    endif
        
    ifeq ($(CONFIG_OCCOAPTINY_ENABLE), y)    
		include $(iot_link_root)/oc/oc_coap/atiny_coap/atiny_coap.mk
    endif

endif