################################################################################
# this is used for compile the OC_MQTT
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################
#NEXT TIME WE SHOULD MOVE THE JSON OUT

ifneq ($(CONFIG_OC_COAP_TYPE),"none")

    OC_COAP_AL_SRC  = ${wildcard $(iot_link_root)/oc/oc_coap/oc_coap_al/*.c}
    C_SOURCES += $(OC_COAP_AL_SRC)	
    
    OC_COAP_AL_INC = -I $(iot_link_root)/oc/oc_coap/oc_coap_al
    C_INCLUDES += $(OC_COAP_AL_INC)	
    
    oc_coap_defs = -D CONFIG_OC_COAP_ENABLE=1
    C_DEFS += $(oc_coap_defs)
    
    
    #you must choose one of the oc coap implement
    include $(iot_link_root)/oc/oc_coap/oc_coap_imp/oc_coap_imp.mk
    
    
    #with coap demo
    include $(SDK_DIR)/demos/oc_coap_demo/oc_coap_demo.mk

endif