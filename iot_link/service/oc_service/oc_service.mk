################################################################################
# this is used for compile the service module
################################################################################

ifeq ($(CONFIG_OC_MQTT_ENABLE),y)
    C_SOURCES += ${wildcard $(iot_link_root)/service/oc_service/oc_mqtt_service/*.c}
    
endif
ifeq ($(CONFIG_OC_COAP_ENABLE),y)
    C_SOURCES += ${wildcard $(iot_link_root)/service/oc_service/oc_coap_service/*.c}
    
endif
ifeq ($(CONFIG_OC_LWM2M_ENABLE),y)
    C_SOURCES += ${wildcard $(iot_link_root)/service/oc_service/oc_lwm2m_service/*.c}
    
endif
C_INCLUDES += -I $(iot_link_root)/service/oc_service/