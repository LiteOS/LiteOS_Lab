################################################################################
# this is used for compile the oc mqtt implement
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################


#uptils now, we only support the atiny_mqtt mode, you could add more implement as you wish

ifeq ($(cfg_oc_mqtt_type),soft)
	include $(iot_link_root)/oc/oc_mqtt/oc_mqtt_imp/atiny_mqtt/atiny_mqtt.mk
else ifeq ($(cfg_oc_mqtt_type),ec20)
	include $(iot_link_root)/oc/oc_mqtt/oc_mqtt_imp/ec20_cefag/ec20_oc.mk
endif 

