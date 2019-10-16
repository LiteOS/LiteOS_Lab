################################################################################
# this is used for compile the mqtt lib
################################################################################

litecoap_src = \
		${wildcard $(TOP_DIR)/iot_link/network/coap/coap_imp/litecoap/src/*.c} \
		${wildcard $(TOP_DIR)/iot_link/network/coap/coap_imp/litecoap/port/*.c}
        C_SOURCES += $(litecoap_src)
        
litecoap_inc = \
        -I $(TOP_DIR)/iot_link/network/coap/coap_imp/litecoap/include
        C_INCLUDES += $(litecoap_inc)
