################################################################################
# this is used for compile the oc mqtt implement
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################


#uptils now, we only support the atiny_mqtt mode, you could add more implement as you wish


cfg_oc_mqtt_type = atiny_mqtt

ifeq ($(cfg_oc_mqtt_type),atiny_mqtt)
	include $(iot_link_root)/oc/oc_mqtt/oc_mqtt_imp/atiny_mqtt/atiny_mqtt.mk
endif 

