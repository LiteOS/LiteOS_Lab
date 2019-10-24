################################################################################
# this is used for compile the OC_MQTT
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################
#NEXT TIME WE SHOULD MOVE THE JSON OUT

ifeq ($(CONFIG_OC_MQTT_ENABLE), y)

    OC_MQTT_AL_SRC  = ${wildcard $(iot_link_root)/oc/oc_mqtt/oc_mqtt_al/*.c}
    C_SOURCES += $(OC_MQTT_AL_SRC)	
    
    OC_MQTT_AL_INC = -I $(iot_link_root)/oc/oc_mqtt/oc_mqtt_al
    C_INCLUDES += $(OC_MQTT_AL_INC)	
    
    oc_mqtt_defs = -D CONFIG_OC_MQTT_ENABLE=0
    C_DEFS += $(oc_mqtt_defs)
    
    ifeq ($(CONFIG_OC_MQTT_TYPE),"soft")
		include $(iot_link_root)/oc/oc_mqtt/atiny_mqtt/atiny_mqtt.mk
    else ifeq ($(CONFIG_OC_MQTT_TYPE),"ec20")
    	include $(iot_link_root)/oc/oc_mqtt/ec20_oc/ec20_oc.mk
    endif 

endif