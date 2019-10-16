################################################################################
# this is used for compile the mqtt lib
################################################################################

loscoap_src = \
		${wildcard $(TOP_DIR)/iot_link/network/coap/coap_imp/litecoap/src/*.c} \
		${wildcard $(TOP_DIR)/iot_link/network/coap/coap_imp/litecoap/port/*.c}
        C_SOURCES += $(loscoap_src)
        
loscoap_inc = \
        -I $(TOP_DIR)/iot_link/network/coap/coap_imp/litecoap/include
        C_INCLUDES += $(loscoap_inc)
