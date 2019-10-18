################################################################################
# this is used for compile the atiny_mqtt
################################################################################
atiny_mqtt_src = ${wildcard $(iot_link_root)/oc/oc_mqtt/atiny_mqtt/*.c}
C_SOURCES += $(atiny_mqtt_src)

atiny_mqtt_inc = -I $(iot_link_root)/oc/oc_mqtt/atiny_mqtt
C_INCLUDES += $(atiny_mqtt_inc)

C_DEFS += -D CONFIG_ATINY_MQTT_ENABLE=1

LDFLAGS += -u _printf_float
