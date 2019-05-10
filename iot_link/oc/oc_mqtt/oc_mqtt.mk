################################################################################
# this is used for compile the OC_MQTT
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################
#NEXT TIME WE SHOULD MOVE THE JSON OUT
OC_MQTT_AL_SRC  = ${wildcard $(TOP_DIR)/iot_link/oc/oc_mqtt/oc_mqtt_al/*.c} \
				  ${wildcard $(TOP_DIR)/components/lib/cJSON/cJSON.c}
C_SOURCES += $(OC_MQTT_AL_SRC)	

OC_MQTT_AL_INC = -I $(TOP_DIR)/iot_link/oc/oc_mqtt/oc_mqtt_al \
			     -I $(TOP_DIR)/iot_link/cJSON 
C_INCLUDES += $(OC_MQTT_AL_INC)	

OC_MQTT_DEFS = -D CFG_OC_MQTT_EN=1    
C_DEFS += $(OC_MQTT_DEFS)


#only implement the mqtt agent, later will add more mqtt method to connect to the huawei iot according to mqtt
WITH_OC_MQTT_AGENT = yes
ifeq ($(WITH_OC_MQTT_AGENT),yes)
	OC_MQTT_AGENT_SRC = ${wildcard $(TOP_DIR)/iot_link/oc/oc_mqtt/oc_mqtt_agent/*.c}
	C_SOURCES += $(OC_MQTT_AGENT_SRC)
	
	OC_MQTT_AGENT_INC = -I $(TOP_DIR)/iot_link/oc/oc_mqtt/oc_mqtt_agent
	C_INCLUDES += $(OC_MQTT_AGENT_INC)
	
	OC_MQTT_AGENT_DEFS = -D CFG_OC_MQTT_AGENT_EN=1    
	C_DEFS += $(OC_MQTT_AGENT_DEFS)
		
endif


#with mqtt demo
WITH_OC_MQTT_DEMO = yes
ifeq ($(WITH_OC_MQTT_DEMO),yes)
	OC_MQTT_DEMO_SRC = ${wildcard $(TOP_DIR)/iot_link/oc/oc_mqtt/oc_mqtt_demo/*.c}
	C_SOURCES += $(OC_MQTT_DEMO_SRC)
	
	OC_MQTT_DEMO_INC = -I $(TOP_DIR)/iot_link/oc/oc_mqtt/oc_mqtt_demo
	C_INCLUDES += $(OC_MQTT_DEMO_INC)
	
	OC_MQTT_DEMO_DEFS = -D CFG_OC_MQTT_DEMO_STATIC_EN=1    
	C_DEFS += $(OC_MQTT_DEMO_DEFS)
		
endif