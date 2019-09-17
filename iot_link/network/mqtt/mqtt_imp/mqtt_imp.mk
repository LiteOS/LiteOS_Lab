################################################################################
# this is used for compile the mqtt lib
################################################################################
#you should choose one mqtt implement, uptils now, we  only adapter the "paho" mqtt				
ifeq ($(CONFIG_MQTT_TYPE), "paho")

	include $(iot_link_root)/network/mqtt/mqtt_imp/mqtt_paho/mqtt_paho.mk
        C_DEFS += -D CONFIG_MQTT_PAHO_ENABLE=1

else ifeq ($(CONFIG_MQTT_TYPE), "sinn")

	include $(iot_link_root)/network/mqtt/mqtt_imp/mqtt_sinn/mqtt_sinn.mk
        C_DEFS += -D CONFIG_MQTT_SINN_ENABLE=1

endif
