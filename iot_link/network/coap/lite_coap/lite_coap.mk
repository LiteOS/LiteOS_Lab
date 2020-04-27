################################################################################
# this is used for compile the mqtt lib
################################################################################

lite_coap_src = \
		${wildcard $(iot_link_root)/network/coap/lite_coap/src/*.c} \
		${wildcard $(iot_link_root)/network/coap/lite_coap/port/*.c}
        C_SOURCES += $(lite_coap_src)
        
lite_coap_inc = \
        -I $(iot_link_root)/network/coap/lite_coap/include
        C_INCLUDES += $(lite_coap_inc)
