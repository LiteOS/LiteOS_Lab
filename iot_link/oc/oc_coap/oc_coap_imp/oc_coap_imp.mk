################################################################################
# this is used for compile the oc mqtt implement
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################


#uptils now, we only support the atiny_mqtt mode, you could add more implement as you wish

ifeq ($(CONFIG_OC_COAP_TYPE),"soft")
	include $(iot_link_root)/oc/oc_coap/oc_coap_imp/atiny_coap/atiny_coap.mk
endif 

