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
    
    OC_MQTT_DEFS = -D CONFIG_OC_MQTT_ENABLE=1
    C_DEFS += $(OC_MQTT_DEFS)
    
#   add the profile  function tools 
    ifeq ($(CONFIG_OC_MQTT_VERSION),"V5")
    	OC_MQTT_PROFILE_SRC  = ${wildcard $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile_v5/*.c}
        OC_MQTT_PROFILE_INC = -I $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile_v5	    	
    else
    	OC_MQTT_PROFILE_SRC  = ${wildcard $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile/*.c}
        OC_MQTT_PROFILE_INC = -I $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile
    endif
    C_SOURCES += $(OC_MQTT_PROFILE_SRC)	
    C_INCLUDES += $(OC_MQTT_PROFILE_INC)	
#   add the implement component 
    ifeq ($(CONFIG_OC_MQTT_TYPE),"ec2x_oc")
    	include $(iot_link_root)/oc/oc_mqtt/ec2x_oc/ec2x_oc.mk
    else ifeq ($(CONFIG_OC_MQTT_TYPE),"soft")
    	ifeq ($(CONFIG_OC_MQTT_VERSION),"V5")
			include $(iot_link_root)/oc/oc_mqtt/oc_mqtt_tiny_v5/oc_mqtt_tiny.mk	
    	else
    		include $(iot_link_root)/oc/oc_mqtt/oc_mqtt_tiny/oc_mqtt_tiny.mk	
    	endif
    endif 

endif