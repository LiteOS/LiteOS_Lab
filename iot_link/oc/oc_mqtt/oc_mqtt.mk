################################################################################
# this is used for compile the OC_MQTT
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################
#NEXT TIME WE SHOULD MOVE THE JSON OUT
OC_MQTT_AL_SRC  = ${wildcard $(iot_link_root)/oc/oc_mqtt/oc_mqtt_al/*.c}
C_SOURCES += $(OC_MQTT_AL_SRC)	

OC_MQTT_AL_INC = -I $(iot_link_root)/oc/oc_mqtt/oc_mqtt_al
C_INCLUDES += $(OC_MQTT_AL_INC)	

OC_MQTT_DEFS = -D CFG_OC_MQTT_EN=1    
C_DEFS += $(OC_MQTT_DEFS)


#you must choose one of the oc mqtt implement
include $(iot_link_root)/oc/oc_mqtt/oc_mqtt_imp/oc_mqtt_imp.mk


#with mqtt demo
include $(iot_link_root)/oc/oc_mqtt/oc_mqtt_demo/oc_mqtt_demo.mk