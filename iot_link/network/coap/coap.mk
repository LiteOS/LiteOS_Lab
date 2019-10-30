################################################################################
# this is used for compile the coap
################################################################################
ifeq ($(CONFIG_COAP_ENABLE), y)

    COAP_MODULE_SOURCE  = ${wildcard $(iot_link_root)/network/coap/coap_al/*.c}
    C_SOURCES += $(COAP_MODULE_SOURCE)
    		
    COAP_MODULE_INC = -I $(iot_link_root)/network/coap/coap_al
    C_INCLUDES += $(COAP_MODULE_INC)
    
    C_DEFS += -D CONFIG_COAP_ENABLE=1
    
    ifeq ($(CONFIG_COAP_TYPE), "lite_coap")
        C_DEFS += -D CONFIG_LITE_COAP_ENABLE=1
        include $(iot_link_root)/network/coap/lite_coap/lite_coap.mk
    else ifeq ($(CONFIG_COAP_TYPE), "libcoap")
        C_DEFS += -D CONFIG_LIBCOAP_ENABLE=1
        include $(iot_link_root)/network/coap/libcoap/libcoap.mk
    endif
    
   
endif