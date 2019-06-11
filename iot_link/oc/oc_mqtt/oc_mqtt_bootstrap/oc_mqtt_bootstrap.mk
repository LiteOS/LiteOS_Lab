################################################################################
# this is used for compile the oc mqtt implement
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################


bootstrap_src = ${wildcard $(iot_link_root)/oc/oc_mqtt/oc_mqtt_bootstrap/*.c} 
C_SOURCES += $(bootstrap_src)


bootstrap_inc = -I $(iot_link_root)/oc/oc_mqtt/oc_mqtt_bootstrap
C_INCLUDES += $(bootstrap_inc)
