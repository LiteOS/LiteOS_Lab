################################################################################
# this is used for compile the oc mqtt implement
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################

#uptils now, we support the mqtt static mode and bs mode

cfg_oc_mqtt_demo_type = oc_mqtt_demo_static
oc_mqtt_demo_src =

ifeq ($(cfg_oc_mqtt_demo_type),oc_mqtt_demo_static)

	oc_mqtt_demo_src = ${wildcard $(iot_link_root)/oc/oc_mqtt/oc_mqtt_demo/oc_mqtt_static.c}

else ifeq ($(cfg_oc_mqtt_demo_type),oc_mqtt_demo_bs)

	oc_mqtt_demo_src = ${wildcard $(iot_link_root)/oc/oc_mqtt/oc_mqtt_demo/oc_mqtt_with_bs.c}

endif

C_SOURCES += $(oc_mqtt_demo_src)

oc_mqtt_demo_inc = -I $(iot_link_root)/oc/oc_mqtt/oc_mqtt_demo
C_INCLUDES += $(oc_mqtt_demo_inc)
