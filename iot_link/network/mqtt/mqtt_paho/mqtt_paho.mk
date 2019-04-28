################################################################################
# this is used for compile the mqtt lib
################################################################################

MQTT_PAHO_SOURCE = \
		${wildcard $(TOP_DIR)/iot_link/network/mqtt/mqtt_paho/mqtt_port/*.c} \
        ${wildcard $(TOP_DIR)/iot_link/network/mqtt/mqtt_paho/mqtt_paho/MQTTPacket/src/*.c} \
        $(TOP_DIR)/iot_link/network/mqtt/mqtt_paho/mqtt_paho/MQTTClient-C/src/MQTTClient.c
        C_SOURCES += $(MQTT_PAHO_SOURCE)
        
MQTT_PAHO_INC = \
        -I $(TOP_DIR)/iot_link/network/mqtt/mqtt_paho/mqtt_paho/MQTTClient-C/src \
		-I $(TOP_DIR)/iot_link/network/mqtt/mqtt_paho/mqtt_paho/MQTTPacket/src \
		-I $(TOP_DIR)/iot_link/network/mqtt/mqtt_paho/mqtt_port 

C_INCLUDES += $(MQTT_PAHO_INC)

MQTT_PAHO_DEF = \
 		-D MQTTCLIENT_PLATFORM_HEADER=mqtt_osdep.h

		C_DEFS += $(MQTT_PAHO_DEF)
