################################################################################
# this is used for compile the oc mqtt implement
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################

#uptils now, we support:"none" "oc_mqtt_demo_static" "oc_mqtt_demo_bs"

ifeq ($(CONFIG_OC_COAP_DEMO_TYPE), "none")
	oc_coap_demo_src =
	oc_coap_demo_inc =
	oc_coap_demo_def =
	
else ifeq ($(CONFIG_OC_COAP_DEMO_TYPE),"oc_coap_demo_nodtls")

	oc_coap_demo_src = ${wildcard $(SDK_DIR)/demos/oc_coap_demo/oc_coap_demo_nodtls/oc_coap_demo_nodtls.c}
	oc_coap_demo_inc = -I $(SDK_DIR)/demos/oc_coap_demo
	oc_coap_demo_def = -D CONFIG_OC_COAP_DEMO_ENABLE=1

else ifeq ($(CONFIG_OC_COAP_DEMO_TYPE),"oc_coap_demo_dtls")

	oc_coap_demo_src = ${wildcard $(SDK_DIR)/demos/oc_coap_demo/oc_coap_demo_dtls/oc_coap_demo_dtls.c}
	oc_coap_demo_inc = -I $(SDK_DIR)/demos/oc_coap_demo
	oc_coap_demo_def = -D CONFIG_OC_COAP_DEMO_ENABLE=1
	
endif

C_SOURCES += $(oc_coap_demo_src)
C_INCLUDES += $(oc_coap_demo_inc)
C_DEFS += $(oc_coap_demo_def)

