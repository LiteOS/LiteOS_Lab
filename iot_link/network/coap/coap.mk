################################################################################
# this is used for compile the coap
################################################################################
ifneq ($(CONFIG_COAP_TYPE),"none")

    COAP_MODULE_SOURCE  = ${wildcard $(iot_link_root)/network/coap/coap_al/*.c}
    C_SOURCES += $(COAP_MODULE_SOURCE)
    		
    COAP_MODULE_INC = -I $(iot_link_root)/network/coap/coap_al
    C_INCLUDES += $(COAP_MODULE_INC)
    
    C_DEFS += -D CONFIG_COAP_ENABLE=1
    
    #you should configure the coap implement method
    include $(iot_link_root)/network/coap/coap_imp/coap_imp.mk

endif