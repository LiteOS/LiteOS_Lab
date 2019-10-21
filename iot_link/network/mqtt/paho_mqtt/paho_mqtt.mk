################################################################################
# this is used for compile the mqtt lib
################################################################################

mqtt_paho_src = \
		${wildcard $(TOP_DIR)/iot_link/network/mqtt/paho_mqtt/port/*.c} \
        ${wildcard $(TOP_DIR)/iot_link/network/mqtt/paho_mqtt/paho/MQTTPacket/src/*.c} \
        $(TOP_DIR)/iot_link/network/mqtt/paho_mqtt/paho/MQTTClient-C/src/MQTTClient.c
        C_SOURCES += $(mqtt_paho_src)
        
mqtt_paho_inc = \
        -I $(TOP_DIR)/iot_link/network/mqtt/paho_mqtt/paho/MQTTClient-C/src \
		-I $(TOP_DIR)/iot_link/network/mqtt/paho_mqtt/paho/MQTTPacket/src \
		-I $(TOP_DIR)/iot_link/network/mqtt/paho_mqtt/port 

C_INCLUDES += $(mqtt_paho_inc)

mqtt_paho_defs = -D MQTTCLIENT_PLATFORM_HEADER=paho_osdepends.h -D CONFIG_MQTT_PAHO_ENABLE=1
C_DEFS += $(mqtt_paho_defs)
