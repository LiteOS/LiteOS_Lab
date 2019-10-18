################################################################################
# this is used for compile the mqtt lib
################################################################################
#you should choose one mqtt implement, uptils now, we  only adapter the "paho" mqtt				
ifeq ($(CONFIG_COAP_TYPE), "litecoap")

	include $(iot_link_root)/network/coap/coap_imp/litecoap/litecoap.mk
 	        
endif