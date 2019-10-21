################################################################################
# this is used for compile the wakaama
################################################################################
WAKAAMA_COAP_SRC = \
        ${wildcard $(iot_link_root)/network/lwm2m/wakaama_lwm2m/core/er-coap-13/er-coap-13.c}
        C_SOURCES += $(WAKAAMA_COAP_SRC)

WAKAMA_LWM2MSRC = \
        ${wildcard $(iot_link_root)/network/lwm2m/wakaama_lwm2m/core/*.c} \
        ${wildcard $(iot_link_root)/network/lwm2m/wakaama_lwm2m/examples/shared/*.c}
        C_SOURCES += $(WAKAMA_LWM2MSRC)
        
WAKAAMA_COAP_INC = \
        -I $(iot_link_root)/network/lwm2m/wakaama_lwm2m/core/er-coap-13
        C_INCLUDES += $(WAKAAMA_COAP_INC)
WAKAMA_LWM2MINC = \
        -I $(iot_link_root)/network/lwm2m/wakaama_lwm2m/core \
        -I $(iot_link_root)/network/lwm2m/wakaama_lwm2m/examples/shared
        C_INCLUDES += $(WAKAMA_LWM2MINC)        
       
C_DEFS += -D LWM2M_LITTLE_ENDIAN -D LWM2M_CLIENT_MODE -D LWM2M_WITH_LOGS
