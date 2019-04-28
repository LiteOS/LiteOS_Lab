################################################################################
# this is used for compile the OC_MQTT
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################
#NEXT TIME WE SHOULD MOVE THE JSON OUT
#OC_MQTT_AL_SRC  = \
#		${wildcard $(TOP_DIR)/components/connectivity/agent_tiny/OC_MQTT/*.c} \
#		${wildcard $(TOP_DIR)/components/lib/cJSON/cJSON.c}
#C_SOURCES += $(OC_MQTT_AL_SRC)	
#
#OC_MQTT_AL_INC = \
#	    -I $(TOP_DIR)/components/connectivity/agent_tiny/OC_MQTT \
#	    -I $(TOP_DIR)/components/lib/cJSON 
OC_MQTT_AL_SRC  = ${wildcard $(TOP_DIR)/iot_link/oc/oc_mqtt/oc_mqtt_al/*.c}
C_SOURCES += $(OC_MQTT_AL_SRC)	

OC_MQTT_AL_INC = -I $(TOP_DIR)/iot_link/oc/oc_mqtt/oc_mqtt_al
C_INCLUDES += $(OC_MQTT_AL_INC)	

OC_MQTT_DEFS = \
     		-D CFG_OC_MQTT_EN=1
    
C_DEFS += $(OC_MQTT_DEFS)


#with mqtt demo
WITH_OC_MQTT_DEMO = yes
OC_MQTT_DEMO_DEFS = \
     		-D CFG_OC_MQTT_DEMO_STATIC=1   
C_DEFS += $(OC_MQTT_DEMO_DEFS)
ifeq ($(WITH_OC_MQTT_DEMO),yes)
	OC_MQTT_DEMO_SRC = ${wildcard $(TOP_DIR)/iot_link/oc/oc_mqtt/oc_mqtt_al/*.c}
	C_SOURCES += $(OC_MQTT_DEMO_SRC)        
endif